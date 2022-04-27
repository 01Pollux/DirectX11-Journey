
#include "helper.hlsli"

HSInput main(VSInput vs_input)
{
	HSInput hs_input;
	hs_input.PosL = vs_input.PosL;
	return hs_input;
}