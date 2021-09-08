#include<complex>
#include<iostream>
using namespace std;
#include<complex>
#include <iostream>
#include <iomanip>
#include <complex>
#include <cmath>
typedef complex<double> dcomp;
//const   complex<double> j(0.0, 1.0);
using namespace std::complex_literals;
typedef complex<double> dcomp;
using namespace std::complex_literals;
const double PI = 3.141592;
class component
{
	string name;
	string nameDepend;
	string type;
	double factor;
	double mag;
	double ang;
	dcomp S;
	int node1;
	int node2;
	double omega;
	int node1_depend;
	int node2_depend;
	double scaler_depend;
public:
	component() {}
	component(int n1, int n2, double m, double a, string t)
	{
		type = t;
		node1 = n1;
		node2 = n2;
		S = complex<double>(m * cos(a * PI / 180),
			m * sin(a * PI / 180));

	}
	component(int n1, int n2, double Factor, string t)
	{
		type = t;
		node1 = n1;
		node2 = n2;
		factor = Factor;
	}
	component(int n1, int n2, string t)
	{
		type = t;
		node1 = n1;
		node2 = n2;
	}
	bool is_used;
	void set_node1(int n1) { node1 = n1; };
	void set_node2(int n2) { node2 = n2; };
	int get_node1() { return node1; };
	int get_node2() { return node2; };
	void set_currentSource(dcomp Is) { S = Is; };
	dcomp get_currentSource() { return S; };

	void set_voltageSource(dcomp Vs) { S = Vs; };
	dcomp get_voltageSource() { return S; };

	void set_factor(double r) { factor = r; };
	double get_factor() { return factor; };

	void setname(string Name)
	{
		name = Name;
	}
	string getname()
	{
		return name;
	}

	void setname_Depend(string NameDepend)
	{
		nameDepend = NameDepend;
	}
	string getname_Depend()
	{
		return nameDepend;
	}

	void setnode1_depend(int N1_D)
	{
		node1_depend = N1_D;
	}
	int getnode1_depend()
	{
		return node1_depend;
	}

	void setnode2_depend(int N2_D)
	{
		node2_depend = N2_D;
	}
	int getnode2_depend()
	{
		return node2_depend;
	}

	void setscaler_depend(double SC)
	{
		scaler_depend = SC;
	}
	double getScaler_depend()
	{
		return scaler_depend;
	}
	void set_mag_ang(double m, double a)
	{

		S = complex<double>(m * cos(a * PI / 180),
			m * sin(a * PI / 180));
	}
	void set_omega(double W)
	{
		omega = W;
	}
	dcomp get_y()
	{
		if (type == "resistance")
		{
			double x = 1 / factor;
			return dcomp(x);
		}
		if (type == "capacitor")
		{
			dcomp x = ((1i * omega * factor));
			return x;
		}
		if (type == "inductor")
		{
			dcomp x = pow((1i * omega * factor), -1);
			return x;
		}
	};
	void set_type(string TYPE) { type = TYPE; };

	string get_type() { return type; };

};

