#include <type_traits>
#include <mipp.h>

#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_scan.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_very_fast.hpp"

#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std_json.hpp"

#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_std.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_SSE.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_AVX.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x4_AVX.hpp"

#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_very_fast.hpp"

#include "Decoder_RSC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_RSC::name   = "Decoder RSC";
const std::string aff3ct::factory::Decoder_RSC::prefix = "dec";

Decoder_RSC::parameters
::parameters(const std::string prefix)
: Decoder::parameters(Decoder_RSC::name, prefix)
{
	this->type   = "BCJR";
	this->implem = "STD";
}

Decoder_RSC::parameters
::~parameters()
{
}

Decoder_RSC::parameters* Decoder_RSC::parameters
::clone() const
{
	return new Decoder_RSC::parameters(*this);
}

void Decoder_RSC::parameters
::get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const
{
	Decoder::parameters::get_description(req_args, opt_args);

	auto p = this->get_prefix();

	req_args.erase({p+"-cw-size", "N"});

	auto* arg_type_type  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-type", "D"})->type);
	auto* arg_range_type = dynamic_cast<tools::Set<std::string>*>(arg_type_type->get_ranges().front());
	arg_range_type->add_options({"BCJR"});

	auto* arg_type_implem  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-implem"})->type);
	auto* arg_range_implem = dynamic_cast<tools::Set<std::string>*>(arg_type_implem->get_ranges().front());
	arg_range_implem->add_options({"GENERIC", "STD", "FAST", "VERY_FAST"});

	opt_args.add(
		{p+"-simd"},
		new tools::Text<>({new tools::Including_set<std::string>({"INTRA", "INTER"})}),
		"the SIMD strategy you want to use.");

	opt_args.add(
		{p+"-max"},
		new tools::Text<>({new tools::Including_set<std::string>({"MAX", "MAXL", "MAXS"})}),
		"the MAX implementation for the nodes.");

	opt_args.add(
		{p+"-no-buff"},
		new tools::None(),
		"does not suppose a buffered encoding.");

	opt_args.add(
		{p+"-poly"},
		new tools::Text<>(),
		"the polynomials describing RSC code, should be of the form \"{A,B}\".");

	opt_args.add(
		{p+"-std"},
		new tools::Text<>({new tools::Including_set<std::string>({"LTE", "CCSDS"})}),
		"select a standard and set automatically some parameters (overwritten with user given arguments).");
}

void Decoder_RSC::parameters
::store(const tools::Argument_map_value &vals)
{
	Decoder::parameters::store(vals);

	auto p = this->get_prefix();

	if(vals.exist({p+"-simd"   })) this->simd_strategy = vals.at({p+"-simd"});
	if(vals.exist({p+"-max"    })) this->max           = vals.at({p+"-max" });
	if(vals.exist({p+"-std"    })) this->standard      = vals.at({p+"-std" });
	if(vals.exist({p+"-no-buff"})) this->buffered      = false;

	if (this->standard == "LTE" && !vals.exist({p+"-poly"}))
		this->poly = {013, 015};

	if (this->standard == "CCSDS" && !vals.exist({p+"-poly"}))
		this->poly = {023, 033};

	if (vals.exist({p+"-poly"}))
	{
		auto poly_str = vals.at({p+"-poly"});

#ifdef _MSC_VER
		sscanf_s   (poly_str.c_str(), "{%o,%o}", &this->poly[0], &this->poly[1]);
#else
		std::sscanf(poly_str.c_str(), "{%o,%o}", &this->poly[0], &this->poly[1]);
#endif
	}

	if (this->poly[0] != 013 || this->poly[1] != 015)
		this->implem = "GENERIC";

	this->tail_length = (int)(2 * std::floor(std::log2((float)std::max(this->poly[0], this->poly[1]))));
	this->N_cw        = 2 * this->K + this->tail_length;
	this->R           = (float)this->K / (float)this->N_cw;
}

void Decoder_RSC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Decoder::parameters::get_headers(headers, full);

	auto p = this->get_prefix();

	if (this->tail_length && full)
		headers[p].push_back(std::make_pair("Tail length", std::to_string(this->tail_length)));

	if (full) headers[p].push_back(std::make_pair("Buffered", (this->buffered ? "on" : "off")));

	if (!this->standard.empty())
		headers[p].push_back(std::make_pair("Standard", this->standard));

	std::stringstream poly;
	poly << "{0" << std::oct << this->poly[0] << ",0" << std::oct << this->poly[1] << "}";
	headers[p].push_back(std::make_pair(std::string("Polynomials"), poly.str()));

	if (!this->simd_strategy.empty())
		headers[p].push_back(std::make_pair(std::string("SIMD strategy"), this->simd_strategy));

	headers[p].push_back(std::make_pair(std::string("Max type"), this->max));
}

template <typename B, typename Q, typename QD, tools::proto_max<Q> MAX1, tools::proto_max<QD> MAX2>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::_build_seq(const std::vector<std::vector<int>> &trellis,
                   std::ostream                  &stream,
             const int                            n_ite) const
{
	if (this->type == "BCJR")
	{
		     if (this->implem == "STD"         ) return new module::Decoder_RSC_BCJR_seq_std             <B,Q,QD,MAX1,MAX2>(this->K, trellis,        this->buffered,         this->n_frames);
		else if (this->implem == "GENERIC"     ) return new module::Decoder_RSC_BCJR_seq_generic_std     <B,Q,QD,MAX1,MAX2>(this->K, trellis,        this->buffered,         this->n_frames);
		else if (this->implem == "GENERIC_JSON") return new module::Decoder_RSC_BCJR_seq_generic_std_json<B,Q,QD,MAX1,MAX2>(this->K, trellis, n_ite, this->buffered, stream, this->n_frames);
		else if (this->implem == "FAST"        ) return new module::Decoder_RSC_BCJR_seq_fast            <B,Q,QD,MAX1,MAX2>(this->K, trellis,        this->buffered,         this->n_frames);
		else if (this->implem == "VERY_FAST"   ) return new module::Decoder_RSC_BCJR_seq_very_fast       <B,Q,QD,MAX1,MAX2>(this->K, trellis,        this->buffered,         this->n_frames);
		else if (this->implem == "SCAN"        ) return new module::Decoder_RSC_BCJR_seq_scan            <B,Q,QD          >(this->K, trellis,        this->buffered,         this->n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q, typename QD, tools::proto_max_i<Q> MAX>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::_build_simd(const std::vector<std::vector<int>> &trellis) const
{
	if (this->type == "BCJR" && this->simd_strategy == "INTER")
	{
		     if (this->implem == "STD"      ) return new module::Decoder_RSC_BCJR_inter_std      <B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames);
		else if (this->implem == "FAST"     ) return new module::Decoder_RSC_BCJR_inter_fast     <B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames);
		else if (this->implem == "VERY_FAST") return new module::Decoder_RSC_BCJR_inter_very_fast<B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames);
	}

	if (this->type == "BCJR" && this->simd_strategy == "INTRA")
	{
		if (this->implem == "STD")
		{
			switch (mipp::nElReg<Q>())
			{
				case 8: return new module::Decoder_RSC_BCJR_intra_std<B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames);
				default:
					break;
			}
		}
		else if (this->implem == "FAST")
		{
#ifdef __AVX__
			switch (mipp::nElReg<Q>())
			{
				case 8:  return new module::Decoder_RSC_BCJR_intra_fast             <B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames); break;
				case 16: return new module::Decoder_RSC_BCJR_inter_intra_fast_x2_AVX<B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames); break;
				case 32: return new module::Decoder_RSC_BCJR_inter_intra_fast_x4_AVX<B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames); break;
				default:
					break;
			}
#else /* NEON and SSE */
			switch (mipp::nElReg<Q>())
			{
				case 8:  return new module::Decoder_RSC_BCJR_intra_fast             <B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames); break;
				case 16: return new module::Decoder_RSC_BCJR_inter_intra_fast_x2_SSE<B,Q,MAX>(this->K, trellis, this->buffered, this->n_frames); break;
				default:
					break;
			}
#endif
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::build(const std::vector<std::vector<int>>   &trellis,
              std::ostream                    &stream,
        const int                              n_ite) const
{
	using QD = typename std::conditional<std::is_same<Q,int8_t>::value,int16_t,Q>::type;

	if (this->simd_strategy.empty())
	{
		     if (this->max == "MAX" ) return _build_seq<B,Q,QD,tools::max       <Q>,tools::max       <QD>>(trellis, stream, n_ite);
		else if (this->max == "MAXS") return _build_seq<B,Q,QD,tools::max_star  <Q>,tools::max_star  <QD>>(trellis, stream, n_ite);
		else if (this->max == "MAXL") return _build_seq<B,Q,QD,tools::max_linear<Q>,tools::max_linear<QD>>(trellis, stream, n_ite);
	}
	else
	{
		     if (this->max == "MAX" ) return _build_simd<B,Q,QD,tools::max_i       <Q>>(trellis);
		else if (this->max == "MAXS") return _build_simd<B,Q,QD,tools::max_star_i  <Q>>(trellis);
		else if (this->max == "MAXL") return _build_simd<B,Q,QD,tools::max_linear_i<Q>>(trellis);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC
::build(const parameters                      &params,
        const std::vector<std::vector<int>>   &trellis,
              std::ostream                    &stream,
        const int                              n_ite)
{
	return params.template build<B,Q>(trellis, stream, n_ite);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::parameters::build<B_8 ,Q_8 >(const std::vector<std::vector<int>>&, std::ostream&, const int) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::parameters::build<B_16,Q_16>(const std::vector<std::vector<int>>&, std::ostream&, const int) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::parameters::build<B_32,Q_32>(const std::vector<std::vector<int>>&, std::ostream&, const int) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::parameters::build<B_64,Q_64>(const std::vector<std::vector<int>>&, std::ostream&, const int) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::build<B_16,Q_16>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::build<B_32,Q_32>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::build<B_64,Q_64>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::parameters::build<B,Q>(const std::vector<std::vector<int>>&, std::ostream&, const int) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::build<B,Q>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int);
#endif
// ==================================================================================== explicit template instantiation
