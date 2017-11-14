#ifndef FACTORY_DECODER_TURBO_HPP
#define FACTORY_DECODER_TURBO_HPP

#include <string>
#include <type_traits>

#include "Module/Decoder/Turbo/Decoder_turbo.hpp"
#include "Module/Decoder/Decoder_SISO.hpp"
#include "Module/Interleaver/Interleaver.hpp"

#include "Factory/Module/Interleaver/Interleaver.hpp"
#include "Factory/Tools/Code/Turbo/Flip_and_check.hpp"
#include "Factory/Tools/Code/Turbo/Scaling_factor.hpp"
#include "Factory/Module/Decoder/RSC/Decoder_RSC.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
struct Decoder_turbo : public Decoder
{
	static const std::string name;
	static const std::string prefix;

	template <class D1 = Decoder_RSC, class D2 = D1>
	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		bool self_corrected = false;
		bool enable_json    = false;
		int  n_ite          = 6;

		// depending parameters
		typename D1   ::parameters *sub1;
		typename D2   ::parameters *sub2;
		Interleaver   ::parameters *itl;
		Scaling_factor::parameters *sf;
		Flip_and_check::parameters *fnc;

		// ---------------------------------------------------------------------------------------------------- METHODS
		parameters(const std::string p = Decoder_turbo::prefix);
		virtual ~parameters();
		Decoder_turbo::parameters<D1,D2>* clone() const;

		virtual std::vector<std::string> get_names      () const;
		virtual std::vector<std::string> get_short_names() const;
		virtual std::vector<std::string> get_prefixes   () const;

		// parameters construction
		void get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const;
		void store          (const tools::Argument_map_value &vals                                 );
		void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_turbo<B,Q>* build(const module::Interleaver<Q>  &itl,
		                                        module::Decoder_SISO<Q> &siso_n,
		                                        module::Decoder_SISO<Q> &siso_i) const;
	};

	template <typename B = int, typename Q = float, class D1 = Decoder_RSC, class D2 = D1>
	static module::Decoder_turbo<B,Q>* build(const parameters<D1,D2>       &params,
	                                         const module::Interleaver<Q>  &itl,
	                                               module::Decoder_SISO<Q> &siso_n,
	                                               module::Decoder_SISO<Q> &siso_i);
};
}
}

#include "Decoder_turbo.hxx"

#endif /* FACTORY_DECODER_TURBO_HPP */
