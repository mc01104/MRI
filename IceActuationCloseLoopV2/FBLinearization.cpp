#include "FBLinearization.h"

FBLinearization::FBLinearization(void)
{
}

FBLinearization::~FBLinearization(void)
{
}



void FBLinearization::setGain(double K)
{
	m_gainLQR = K;
}
double FBLinearization::getGain()
{
	return m_gainLQR;
}
double FBLinearization::getMeasuredAngularVelocity()
{
	m_angularVelocity = (m_newAngularPosition - m_oldAngularPosition)/m_deltaT; // rad/seconds
	return m_angularVelocity;
}
