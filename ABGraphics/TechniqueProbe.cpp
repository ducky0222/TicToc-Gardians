#include "pch.h"

#include "TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech_in)
{
	mpTechnique = pTech_in;
	mTechniqueIndex++;

	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep_in)
{
	mpStep = pStep_in;
	mStepIndex++;
	OnSetStep();
}

bool TechniqueProbe::VisitBuffer(Dcb::Buffer& buf)
{
	mBufIndex++;

	return OnVisitBuffer(buf);
}
