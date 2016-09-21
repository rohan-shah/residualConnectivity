#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_TYPES_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_TYPES_HEADER_GUARD
#include <vector>
#include "depth_first_search_restricted.hpp"
#include "context.h"
#include "conditionArticulation.h"
#include "conditionalPoissonBase.h"
namespace residualConnectivity
{
	namespace obs
	{
		struct basicConstructorType
		{
		};
		struct withWeightConstructorType
		{
		public:
			withWeightConstructorType();
			mpfr_class weight;
		};
		struct weightAndCountConstructorType
		{
			weightAndCountConstructorType()
			{}
			int count;
			mpfr_class weight;
		};
		struct articulationConditioningSameCountImportanceConstructorType
		{
			articulationConditioningSameCountImportanceConstructorType()
			{}
			int count;
			mpfr_class weight;
			const std::vector<double>* importanceProbabilities;
		};
	}
	namespace subObs
	{
		struct basicConstructorType
		{
		public:
			basicConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack;
		};
		struct withWeightConstructorType
		{
		public:
			withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack;
			boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack;
			mpfr_class weight;
		};
		struct articulationConditioningForResamplingConstructorType : public withWeightConstructorType
		{
			articulationConditioningForResamplingConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack);
		};
		struct articulationConditioningSameCountConstructorType : public articulationConditioningForResamplingConstructorType
		{
			articulationConditioningSameCountConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack);
			int count;
		};
		struct articulationConditioningSameCountImportanceConstructorType
		{
			articulationConditioningSameCountImportanceConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
				:components(components), stack(stack), filteredGraphStack(filteredGraphStack)
			{}
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack;
			boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack;
			sampling::conditionalPoissonArgs samplingArgsOriginal, samplingArgsImportance;
			int count;
			mpfr_class weight;
			const std::vector<double>* importanceProbabilities;
		private:
			articulationConditioningSameCountImportanceConstructorType& operator=(const articulationConditioningSameCountImportanceConstructorType&);
			articulationConditioningSameCountImportanceConstructorType(const articulationConditioningSameCountImportanceConstructorType&);
		};
	}
}
#endif
