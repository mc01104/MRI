

#ifndef __FBLINEARIZATION__
#define __FBLINEARIZATION__

class FBLinearization
{
public:
	FBLinearization(void);
	~FBLinearization(void);

	double getGain();
	double setGain();

	double getAlpha();
	double getBeta();

	double getMeasuredAngularVelocity();
	double getEstimatedAngularVelocity();



private:
	rotor	m_rotor1;
	rotor	m_rotor2;

	double	m_alpha;
	double	m_beta;

	double	m_gainLQR;

	double m_oldAngularPosition; // Rad
	double m_newAngularPosition; // Rad
	double m_angularVelocity;    // Rad/seconds




};

#endif