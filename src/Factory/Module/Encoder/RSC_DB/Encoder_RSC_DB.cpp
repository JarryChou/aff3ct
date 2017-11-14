#include <algorithm>

#include "Tools/Exception/exception.hpp"

#include "Encoder_RSC_DB.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_RSC_DB::name   = "Encoder RSC DB";
const std::string aff3ct::factory::Encoder_RSC_DB::prefix = "enc";

Encoder_RSC_DB::parameters
::parameters(const std::string prefix)
: Encoder::parameters(Encoder_RSC_DB::name, prefix)
{
	this->type = "RSC_DB";
}

Encoder_RSC_DB::parameters
::~parameters()
{
}

Encoder_RSC_DB::parameters* Encoder_RSC_DB::parameters
::clone() const
{
	return new Encoder_RSC_DB::parameters(*this);
}

void Encoder_RSC_DB::parameters
::get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const
{
	Encoder::parameters::get_description(req_args, opt_args);

	auto p = this->get_prefix();

	req_args.erase({p+"-cw-size", "N"});

	auto* arg_type_type  = dynamic_cast<tools::Argument_type_limited<int>*>(opt_args.at({p+"-type"})->type);
	auto* arg_range_type = dynamic_cast<tools::Set<std::string>*>(arg_type_type->get_ranges().front());
	arg_range_type->add_options({"RSC_DB"});

	opt_args.add(
		{p+"-std"},
		new tools::Text<>({new tools::Including_set<std::string>({"DVB", "RCS2"})}),
		"select a standard and set automatically some parameters (overwritten with user given arguments).");

	opt_args.add(
		{p+"-no-buff"},
		new tools::None(),
		"disable the buffered encoding.");
}

void Encoder_RSC_DB::parameters
::store(const tools::Argument_map_value &vals)
{
	Encoder::parameters::store(vals);

	auto p = this->get_prefix();

	if(vals.exist({p+"-no-buff"})) this->buffered = false;
	if(vals.exist({p+"-std"    })) this->standard = vals.at({p+"-std"});

	this->N_cw = 2 * this->K;
	this->R    = (float)this->K / (float)this->N_cw;
}

void Encoder_RSC_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Encoder::parameters::get_headers(headers, full);

	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Buffered", (this->buffered ? "on" : "off")));

	if (!this->standard.empty())
		headers[p].push_back(std::make_pair("Standard", this->standard));
}

template <typename B>
module::Encoder_RSC_DB<B>* Encoder_RSC_DB::parameters
::build() const
{
	if (this->type == "RSC_DB") return new module::Encoder_RSC_DB<B>(this->K, this->N_cw, this->standard, this->buffered, this->n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_RSC_DB<B>* Encoder_RSC_DB
::build(const parameters &params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_RSC_DB<B_8 >* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_8 >() const;
template aff3ct::module::Encoder_RSC_DB<B_16>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_16>() const;
template aff3ct::module::Encoder_RSC_DB<B_32>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_32>() const;
template aff3ct::module::Encoder_RSC_DB<B_64>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_64>() const;
template aff3ct::module::Encoder_RSC_DB<B_8 >* aff3ct::factory::Encoder_RSC_DB::build<B_8 >(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_16>* aff3ct::factory::Encoder_RSC_DB::build<B_16>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_32>* aff3ct::factory::Encoder_RSC_DB::build<B_32>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_64>* aff3ct::factory::Encoder_RSC_DB::build<B_64>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
#else
template aff3ct::module::Encoder_RSC_DB<B>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B>() const;
template aff3ct::module::Encoder_RSC_DB<B>* aff3ct::factory::Encoder_RSC_DB::build<B>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
#endif
// ==================================================================================== explicit template instantiation
