#include <sstream>

#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/NO/Decoder_NO.hpp"

#include "Decoder_NO.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_NO::name   = "Decoder NO";
const std::string aff3ct::factory::Decoder_NO::prefix = "dec";

Decoder_NO::parameters
::parameters(const std::string prefix)
: Decoder::parameters(Decoder_NO::name, prefix)
{
	this->type   = "NONE";
	this->implem = "HARD_DECISION";
}

Decoder_NO::parameters
::~parameters()
{
}

Decoder_NO::parameters* Decoder_NO::parameters
::clone() const
{
	return new Decoder_NO::parameters(*this);
}

void Decoder_NO::parameters
::get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const
{
	Decoder::parameters::get_description(req_args, opt_args);

	auto p = this->get_prefix();

	auto* arg_type_type  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-type", "D"})->type);
	auto* arg_range_type = dynamic_cast<tools::Set<std::string>*>(arg_type_type->get_ranges().front());
	arg_range_type->add_options({"NONE"});

	auto* arg_type_implem  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-implem"})->type);
	auto* arg_range_implem = dynamic_cast<tools::Set<std::string>*>(arg_type_implem->get_ranges().front());
	arg_range_implem->add_options({"HARD_DECISION"});
}

void Decoder_NO::parameters
::store(const tools::Argument_map_value &vals)
{
	Decoder::parameters::store(vals);
}

void Decoder_NO::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Decoder::parameters::get_headers(headers, full);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_NO::parameters
::build() const
{
	if (this->type == "NONE" && this->implem == "HARD_DECISION") return new module::Decoder_NO<B,Q>(this->K, this->n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_NO
::build(const parameters &params)
{
	return params.template build<B,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_NO::parameters::build<B_8 ,Q_8 >() const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_NO::parameters::build<B_16,Q_16>() const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_NO::parameters::build<B_32,Q_32>() const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_NO::parameters::build<B_64,Q_64>() const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_NO::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_NO::parameters&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_NO::build<B_16,Q_16>(const aff3ct::factory::Decoder_NO::parameters&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_NO::build<B_32,Q_32>(const aff3ct::factory::Decoder_NO::parameters&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_NO::build<B_64,Q_64>(const aff3ct::factory::Decoder_NO::parameters&);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_NO::parameters::build<B,Q>() const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_NO::build<B,Q>(const aff3ct::factory::Decoder_NO::parameters&);
#endif
// ==================================================================================== explicit template instantiation
