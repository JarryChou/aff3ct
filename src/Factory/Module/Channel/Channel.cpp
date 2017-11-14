#include "Tools/Exception/exception.hpp"

#include "Module/Channel/NO/Channel_NO.hpp"
#include "Module/Channel/User/Channel_user.hpp"
#include "Module/Channel/AWGN/Channel_AWGN_LLR.hpp"
#include "Module/Channel/Rayleigh/Channel_Rayleigh_LLR.hpp"
#include "Module/Channel/Rayleigh/Channel_Rayleigh_LLR_user.hpp"

#include "Tools/Algo/Noise/Standard/Noise_std.hpp"
#include "Tools/Algo/Noise/Fast/Noise_fast.hpp"
#ifdef CHANNEL_MKL
#include "Tools/Algo/Noise/MKL/Noise_MKL.hpp"
#endif
#ifdef CHANNEL_GSL
#include "Tools/Algo/Noise/GSL/Noise_GSL.hpp"
#endif

#include "Channel.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Channel::name   = "Channel";
const std::string aff3ct::factory::Channel::prefix = "chn";

Channel::parameters
::parameters(const std::string prefix)
: Factory::parameters(Channel::name, Channel::name, prefix)
{
}

Channel::parameters
::~parameters()
{
}

Channel::parameters* Channel::parameters
::clone() const
{
	return new Channel::parameters(*this);
}

void Channel::parameters
::get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const
{
	auto p = this->get_prefix();

	req_args.add(
		{p+"-fra-size", "N"},
		new tools::Integer<>({new tools::Positive<int>(), new tools::Non_zero<int>()}),
		"number of symbols by frame.");

	opt_args.add(
		{p+"-fra", "F"},
		new tools::Integer<>({new tools::Positive<int>(), new tools::Non_zero<int>()}),
		"set the number of inter frame level to process.");

	opt_args.add(
		{p+"-type"},
		new tools::Text<>({new tools::Including_set<std::string>({"NO", "USER", "AWGN", "RAYLEIGH", "RAYLEIGH_USER"})}),
		"type of the channel to use in the simulation.");

	opt_args.add(
		{p+"-implem"},
		new tools::Text<>({new tools::Including_set<std::string>({"STD", "FAST"})}),
		"select the implementation of the algorithm to generate noise.");

#ifdef CHANNEL_GSL
	auto* arg_type_gsl  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-implem"})->type);
	auto* arg_range_gsl = dynamic_cast<tools::Set<std::string>*>(arg_type_gsl->get_ranges().front());
	arg_range_gsl->add_options({"GSL"});
#endif
#ifdef CHANNEL_MKL
	auto* arg_type_mkl  = dynamic_cast<tools::Argument_type_limited<std::string>*>(opt_args.at({p+"-implem"})->type);
	auto* arg_range_mkl = dynamic_cast<tools::Set<std::string>*>(arg_type_mkl->get_ranges().front());
	arg_range_mkl->add_options({"MKL"});
#endif

	opt_args.add(
		{p+"-path"},
		new tools::Text<>(),
		"path to a noisy file, to use with \"--chn-type USER\" or to a gains file (used with \"--chn-type RAYLEIGH_USER\").");

	opt_args.add(
		{p+"-blk-fad"},
		new tools::Text<>({new tools::Including_set<std::string>({"NO", "FRAME", "ONETAP"})}),
		"block fading policy for the RAYLEIGH channel.");

	opt_args.add(
		{p+"-sigma"},
		new tools::Real<>({new tools::Positive<float>(), new tools::Non_zero<float>()}),
		"noise variance value.");

	opt_args.add(
		{p+"-seed", "S"},
		new tools::Integer<>({new tools::Positive<int>()}),
		"seed used to initialize the pseudo random generators.");

	opt_args.add(
		{p+"-add-users"},
		new tools::None(),
		"add all the users (= frames) before generating the noise.");

	opt_args.add(
		{p+"-complex"},
		new tools::None(),
		"enable complex noise generation.");

	opt_args.add(
		{p+"-gain-occur"},
		new tools::Integer<>({new tools::Positive<int>(), new tools::Non_zero<int>()}),
		"the number of times a gain is used on consecutive symbols (used with \"--chn-type RAYLEIGH_USER\").");
}

void Channel::parameters
::store(const tools::Argument_map_value &vals)
{
	auto p = this->get_prefix();

	if(vals.exist({p+"-fra-size", "N"})) this->N            = vals.to_int({p+"-fra-size", "N"});
	if(vals.exist({p+"-fra",      "F"})) this->n_frames     = vals.to_int({p+"-fra",      "F"});
	if(vals.exist({p+"-seed",     "S"})) this->seed         = vals.to_int({p+"-seed",     "S"});
	if(vals.exist({p+"-gain-occur"   })) this->gain_occur   = vals.to_int({p+"-gain-occur"   });
	if(vals.exist({p+"-type"         })) this->type         = vals.at    ({p+"-type"         });
	if(vals.exist({p+"-implem"       })) this->implem       = vals.at    ({p+"-implem"       });
	if(vals.exist({p+"-path"         })) this->path         = vals.at    ({p+"-path"         });
	if(vals.exist({p+"-blk-fad"      })) this->block_fading = vals.at    ({p+"-blk-fad"      });
	if(vals.exist({p+"-add-users"    })) this->add_users    = true;
	if(vals.exist({p+"-complex"      })) this->complex      = true;
	if(vals.exist({p+"-sigma"        })) this->sigma        = vals.to_float({p+"-sigma"      });
}

void Channel::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Type",           this->type  ));
	headers[p].push_back(std::make_pair("Implementation", this->implem));

	if (full) headers[p].push_back(std::make_pair("Frame size (N)", std::to_string(this->N)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));

	if (this->sigma != -1.f)
		headers[p].push_back(std::make_pair("Sigma value", std::to_string(this->sigma)));

	if (this->type == "USER" || this->type == "RAYLEIGH_USER")
		headers[p].push_back(std::make_pair("Path", this->path));

	if (this->type == "RAYLEIGH_USER")
		headers[p].push_back(std::make_pair("Gain occurrences", std::to_string(this->gain_occur)));

	if (this->type.find("RAYLEIGH") != std::string::npos)
		headers[p].push_back(std::make_pair("Block fading policy", this->block_fading));

	if ((this->type != "NO" && this->type != "USER") && full)
		headers[p].push_back(std::make_pair("Seed", std::to_string(this->seed)));

	headers[p].push_back(std::make_pair("Complex", this->complex ? "on" : "off"));
	headers[p].push_back(std::make_pair("Add users", this->add_users ? "on" : "off"));
}

template <typename R>
module::Channel<R>* Channel::parameters
::build() const
{
	tools::Noise<R>* n = nullptr;
	     if (implem == "STD" ) n = new tools::Noise_std <R>(seed);
	else if (implem == "FAST") n = new tools::Noise_fast<R>(seed);
#ifdef CHANNEL_MKL
	else if (implem == "MKL" ) n = new tools::Noise_MKL <R>(seed);
#endif
#ifdef CHANNEL_GSL
	else if (implem == "GSL" ) n = new tools::Noise_GSL <R>(seed);
#endif
	else
		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);


	     if (type == "AWGN"         ) return new module::Channel_AWGN_LLR         <R>(N,                            n, add_users, sigma, n_frames);
	else if (type == "RAYLEIGH"     ) return new module::Channel_Rayleigh_LLR     <R>(N, complex,                   n, add_users, sigma, n_frames);
	else if (type == "RAYLEIGH_USER") return new module::Channel_Rayleigh_LLR_user<R>(N, complex, path, gain_occur, n, add_users, sigma, n_frames);
	else
	{
		module::Channel<R>* c = nullptr;
		     if (type == "USER") c = new module::Channel_user<R>(N, path, add_users, n_frames);
		else if (type == "NO"  ) c = new module::Channel_NO  <R>(N,       add_users, n_frames);

		delete n;

		if (c) return c;

		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
	}
}

template <typename R>
module::Channel<R>* Channel
::build(const parameters &params)
{
	return params.template build<R>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::parameters::build<R_32>() const;
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::parameters::build<R_64>() const;
template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::build<R_32>(const aff3ct::factory::Channel::parameters&);
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::build<R_64>(const aff3ct::factory::Channel::parameters&);
#else
template aff3ct::module::Channel<R>* aff3ct::factory::Channel::parameters::build<R>() const;
template aff3ct::module::Channel<R>* aff3ct::factory::Channel::build<R>(const aff3ct::factory::Channel::parameters&);
#endif
// ==================================================================================== explicit template instantiation
