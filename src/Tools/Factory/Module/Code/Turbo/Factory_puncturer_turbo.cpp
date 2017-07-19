#include "Tools/general_utils.h"
#include "Tools/Exception/exception.hpp"

#include "Module/Puncturer/NO/Puncturer_NO.hpp"
#include "Module/Puncturer/Turbo/Puncturer_turbo.hpp"

#include "Factory_puncturer_turbo.hpp"

using namespace aff3ct::module;
using namespace aff3ct::tools;

template <typename B, typename Q>
Puncturer<B,Q>* Factory_puncturer_turbo
::build(const parameters &params)
{
	if (params.type == "TURBO") return new Puncturer_turbo<B,Q>(params.K, params.N, params.tail_length, params.pattern, params.buffered, params.n_frames);

	throw cannot_allocate(__FILE__, __LINE__, __func__);
}

void Factory_puncturer_turbo
::build_args(Arguments_reader::arg_map &req_args, Arguments_reader::arg_map &opt_args)
{
	Factory_puncturer::build_args(req_args, opt_args);
	req_args.erase({"pct-fra-size", "N"});

	opt_args[{"pct-type"}][2] += ", TURBO";

	opt_args[{"pct-pattern"}] =
		{"string",
		 "puncturing pattern for the turbo encoder (ex: \"11,10,01\")."};

	opt_args[{"pct-tail-length"}] =
		{"positive_int",
		 "total number of tail bits at the end of the frame."};

	opt_args[{"pct-no-buff"}] =
		{"",
		 "does not suppose a buffered encoding."};
}

int compute_N(const int K, const int tail_bits,  const std::string pattern)
{
	std::vector<std::vector<bool>> pattern_bits(3);

	if (tail_bits < 0)
	{
		std::stringstream message;
		message << "'tail_bits' has to be positive ('tail_bits' = " << tail_bits << ").";
		throw invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	auto str_array = string_split(pattern, ',');

	if (str_array.size() != 3)
	{
		std::stringstream message;
		message << "'pattern' should give 3 different set delimited by a comma ('pattern' = " << pattern
		        << ", 'str_array.size()' = " << str_array.size() << ").";
		throw invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (str_array[0].size() != str_array[1].size() || str_array[0].size() != str_array[2].size())
	{
		std::stringstream message;
		message << "'pattern' sets have to contains an equal number of bits ('pattern' = " << pattern
		        << ", 'str_array[0].size()' = " << str_array[0].size()
		        << ", 'str_array[1].size()' = " << str_array[1].size()
		        << ", 'str_array[2].size()' = " << str_array[2].size() << ").";
		throw invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	auto all_one = true;
	for (auto i = 0; i < 3; i++)
		for (auto j = 0; j < (int)str_array[i].size(); j++)
			all_one = str_array[i][j] != '1' ? false : all_one;
	if (all_one)
		return 3 * K + tail_bits;

	auto period = (int)str_array[0].size();

	if (K % period)
	{
		std::stringstream message;
		message << "'period' has to be a multiple of 'K' ('period' = " << period << ", 'K' = " << K << ").";
		throw invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	pattern_bits[0].resize(period);
	pattern_bits[1].resize(period);
	pattern_bits[2].resize(period);

	for (auto i = 0; i < 3; i++)
		for (auto j = 0; j < period; j++)
		{
			char c[2] = {str_array[i][j], '\0'};
			pattern_bits[i][j] = std::stoi(std::string(c)) ? true : false;
		}

	auto bit_sys_count = 0; for (auto j = 0; j < period; j++) bit_sys_count += pattern_bits[0][j] ? 1 : 0;
	auto bit_pa1_count = 0; for (auto j = 0; j < period; j++) bit_pa1_count += pattern_bits[1][j] ? 1 : 0;
	auto bit_pa2_count = 0; for (auto j = 0; j < period; j++) bit_pa2_count += pattern_bits[2][j] ? 1 : 0;

	auto bit_count = bit_sys_count + bit_pa1_count + bit_pa2_count;

	auto N = (K / period) * bit_count + tail_bits;

	return N;
}

void Factory_puncturer_turbo
::store_args(const Arguments_reader& ar, parameters &params)
{
	params.type = "TURBO";

	Factory_puncturer::store_args(ar, params);

	if(ar.exist_arg({"pct-pattern"    })) params.pattern     = ar.get_arg    ({"pct-pattern"    });
	if(ar.exist_arg({"pct-tail-length"})) params.tail_length = ar.get_arg_int({"pct-tail-length"});
	if(ar.exist_arg({"pct-no-buff"    })) params.buffered    = false;

	params.N_cw = 3 * params.K + params.tail_length;
	params.N    = compute_N(params.K, params.tail_length, params.pattern);
	params.R    = (float)params.K / (float)params.N;

	if (params.N == params.N_cw)
		params.type = "NO";
}

void Factory_puncturer_turbo
::group_args(Arguments_reader::arg_grp& ar)
{
	Factory_puncturer::group_args(ar);
}

void Factory_puncturer_turbo
::header(params_list& head_pct, const parameters& params)
{
	Factory_puncturer::header(head_pct, params);

	if (params.type != "NO")
	{
		head_pct.push_back(std::make_pair(std::string("Pattern"), std::string("{" + params.pattern) + "}"));
		head_pct.push_back(std::make_pair(std::string("Tail length"), std::to_string(params.tail_length)));
		head_pct.push_back(std::make_pair(std::string("Buffered"), params.buffered ? "on" : "off"));
	}
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::tools::Factory_puncturer_turbo::build<B_8 ,Q_8 >(const aff3ct::tools::Factory_puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::tools::Factory_puncturer_turbo::build<B_16,Q_16>(const aff3ct::tools::Factory_puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::tools::Factory_puncturer_turbo::build<B_32,Q_32>(const aff3ct::tools::Factory_puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::tools::Factory_puncturer_turbo::build<B_64,Q_64>(const aff3ct::tools::Factory_puncturer_turbo::parameters&);
#else
template aff3ct::module::Puncturer<B,Q>* aff3ct::tools::Factory_puncturer_turbo::build<B,Q>(const aff3ct::tools::Factory_puncturer_turbo::parameters&);
#endif
// ==================================================================================== explicit template instantiation
