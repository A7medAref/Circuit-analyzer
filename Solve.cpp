#include<iostream>
using namespace std;
#include<complex>
#include <Eigen/Eigen>
#include <iomanip>
#include <complex>
#include <cmath>
#include"component.h"
#include<fstream>
//#include "inductor.h"
typedef complex<double> dcomp;
const   complex<double> j(0.0, 1.0);
using namespace std::complex_literals;
typedef complex<double> dcomp;
using namespace std::complex_literals;
using namespace std;

using namespace Eigen;
void is_voltage(component* Components[], int countcomponent, int Nodes_num, bool* Flags) //flags is equal number of nodes 
{
    for (int i = 0; i < Nodes_num; i++)
    {
        int get = 0;
        for (int j = 0; j < countcomponent; j++)
        {
            if (Components[j]->get_type() == "VoltageSource" && (Components[j]->get_node1() == i + 1 || Components[j]->get_node2() == i + 1))
            {
                Flags[i] = true; get = 1;
            }
            else if (Components[j]->get_type() == "Dep_VoltageSource" && (Components[j]->get_node1() == i + 1 || Components[j]->get_node2() == i + 1))
            {
                Flags[i] = true; get = 1;
            }
            if (get == 1) {}
            else
            {
                Flags[i] = false;
            }
        }
    }

}
int main()
{
    component* components[40];
    string Type; string name_ob, dependOn;
    int N1, N2, Count = 0;
    double j, w, mag, deg;
    double n1_d, n2_d, f;
    bool Cont = true;
    ifstream input("netlist.txt"); int CountComponent = 0;
    if (input.is_open())
    {

        input >> Type;
        input >> w;
        for (int i = 0; Cont; i++)
        {
            CountComponent++;
            input >> Type;
            if (Type == "stop")
            {
                CountComponent--;
                Cont = false;
            }
            if (Type == "vsrc")
            {
                components[i] = new component;
                components[i]->set_type("VoltageSource");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> mag >> deg;
                components[i]->set_mag_ang(mag, deg);
                components[i]->set_node1(N1); components[i]->set_node2(N2);
            }
            if (Type == "isrc")
            {
                components[i] = new component;
                components[i]->set_type("CurrentSource");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> mag >> deg;
                components[i]->set_mag_ang(mag, deg);
                components[i]->set_node1(N1); components[i]->set_node2(N2);
            }
            if (Type == "res")
            {
                components[i] = new component;
                components[i]->set_type("resistance");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> j;
                components[i]->set_factor(j);
                components[i]->set_node1(N1); components[i]->set_node2(N2);
                components[i]->set_omega(w);
            }
            if (Type == "cap")
            {
                components[i] = new component;
                components[i]->set_type("capacitor");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> j;
                components[i]->set_factor(j);
                components[i]->set_node1(N1); components[i]->set_node2(N2);
                components[i]->set_omega(w);
            }
            if (Type == "ind")
            {
                components[i] = new component;
                components[i]->set_type("inductor");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> j;
                components[i]->set_factor(j);
                components[i]->set_node1(N1); components[i]->set_node2(N2);
                components[i]->set_omega(w);
            }
            if (Type == "vcvs")
            {
                components[i] = new component;
                components[i]->set_type("Dep_VoltageSource");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> n1_d >> n2_d >> f;
                components[i]->set_node1(N1); components[i]->set_node2(N2);
                components[i]->setnode1_depend(N1); components[i]->setnode2_depend(N2);
                components[i]->setscaler_depend(f);
            }
            if (Type == "cccs")
            {
                components[i] = new component;
                components[i]->set_type("Dep_CurrentSource");
                input >> name_ob;
                components[i]->setname(name_ob);
                input >> N1 >> N2 >> n1_d >> n2_d >> dependOn >> f;
                components[i]->set_node1(N1); components[i]->set_node2(N2);
                components[i]->setnode1_depend(n1_d); components[i]->setnode2_depend(n2_d);
                components[i]->setscaler_depend(f); components[i]->setname_Depend(dependOn);
            }
        }
    }
//////////

 /// 
 int nodes_num = 0;
    for (int i = 0; i < CountComponent; ++i)
    {
        //components[i]->is_used = false;
        if (components[i]->get_node1() > nodes_num)
            nodes_num = components[i]->get_node1();
        if (components[i]->get_node2() > nodes_num)
            nodes_num = components[i]->get_node2();
    }
    Eigen::MatrixXcd Ys(nodes_num, nodes_num);
    Eigen::MatrixXcd VS(nodes_num, 1);
    Eigen::MatrixXcd IS(nodes_num, 1);
    Ys.setZero();
    bool* ignore = new bool[nodes_num];

    bool* flags = new bool[nodes_num];
    int node1_super; int node2_super; 
    is_voltage(components, CountComponent, nodes_num, flags);
    bool is_super;


    for (int i = 0; i < nodes_num; ++i)
        for (int j = 0; j < nodes_num; ++j)
        {
            Ys(i, j).imag(0), Ys(i, j).real(0);
            ignore[i] = false;
        }

    for (int i = 0; i < nodes_num; ++i)
    {
        VS(i, 0).imag(0);
        VS(i, 0).real(0);
        IS(i, 0).imag(0);
        IS(i, 0).real(0);
    }

    for (int i = 0; i < nodes_num; i++)
    {
        if (flags[i] == false)
        {
            for (int k = 0; k < CountComponent; k++)
            {
                if (components[k]->get_node1() == i + 1 || components[k]->get_node2() == i + 1)
                {
                    if (components[k]->get_type() == "resistance" || components[k]->get_type() == "capacitor" || components[k]->get_type() == "inductor")
                    {
                        if (components[k]->get_node1() == i + 1)
                        {
                            if (components[k]->get_node2() == 0)
                            {
                                Ys(components[k]->get_node1() - 1, components[k]->get_node1() - 1) += components[k]->get_y();
                            }
                            else
                            {
                                Ys(components[k]->get_node1() - 1, components[k]->get_node1() - 1) += components[k]->get_y();
                                Ys(components[k]->get_node1() - 1, components[k]->get_node2() - 1) -= components[k]->get_y();
                            }
                        }

                        if (components[k]->get_node2() == i + 1)
                        {
                            if (components[k]->get_node1() == 0)
                            {
                                Ys(components[k]->get_node2() - 1, components[k]->get_node2() - 1) += components[k]->get_y();
                            }
                            else
                            {
                                Ys(components[k]->get_node2() - 1, components[k]->get_node2() - 1) += components[k]->get_y();
                                Ys(components[k]->get_node2() - 1, components[k]->get_node1() - 1) -= components[k]->get_y();
                            }
                        }
                    }
                    if (components[k]->get_type() == "CurrentSource")
                    {
                        if (components[k]->get_node1() == 0)
                        {
                            IS(components[k]->get_node2() - 1, 0) -= components[k]->get_currentSource();
                        }
                        else if (components[i]->get_node2() == 0)
                        {
                            IS(components[k]->get_node1() - 1, 0) += components[k]->get_currentSource();
                        }
                        else if (components[k]->get_node1() == i + 1)
                        {
                            IS(components[k]->get_node1() - 1, 0) += components[k]->get_currentSource();
                        }
                        else if (components[k]->get_node2() == i + 1)
                        {
                            IS(components[k]->get_node2() - 1, 0) -= components[k]->get_currentSource();
                        }
                    }
                    ///////////////////////////////////////
                    if (components[k]->get_type() == "Dep_CurrentSource")
                    {
                        int position_of_dependent;
                        for (int h = 0; h < CountComponent; h++)
                        {
                            if (components[h]->getname() == components[k]->getname_Depend())
                            {
                                position_of_dependent = h;
                            }
                        }////in case of node depend on = 0
                        if (components[k]->getnode1_depend() != 0 && components[k]->getnode2_depend() != 0)
                        {
                            if (components[k]->get_node1() == i + 1)
                            {
                                Ys(components[k]->get_node1() - 1, components[k]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                Ys(components[k]->get_node1() - 1, components[k]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                            }
                            else
                            {
                                Ys(components[k]->get_node2() - 1, components[k]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                Ys(components[k]->get_node2() - 1, components[k]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                            }
                        }
                            if (components[k]->getnode1_depend() == 0)
                            {
                                if (components[k]->get_node1() == i + 1)
                                {
                                    Ys(components[k]->get_node1() - 1, components[k]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(components[k]->get_node2() - 1, components[k]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                }
                            }
                            if (components[k]->getnode2_depend() == 0)
                            {
                                if (components[k]->get_node1() == i + 1)
                                {
                                    Ys(components[k]->get_node1() - 1, components[k]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(components[k]->get_node2() - 1, components[k]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[k]->getScaler_depend();
                                }
                            }
                    }
                    ////////////////////////////////////////////////
                }
            }
        }

        if (flags[i] == true && ignore[i] == false)
        {


            for (int b = 0; b < CountComponent; b++)
            {
                if (components[b]->get_type() == "VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                {
                    node1_super = components[b]->get_node1();
                    node2_super = components[b]->get_node2();
                    if (node1_super == 0 || node2_super == 0)
                        is_super = false;
                    else is_super = true;
                }
                if (components[b]->get_type() == "Dep_VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                {
                    node1_super = components[b]->get_node1();
                    node2_super = components[b]->get_node2();
                    if (node1_super == 0 || node2_super == 0)
                        is_super = false;
                    else is_super = true;
                }
            }
            if (is_super == false)
            {
                for (int b = 0; b < CountComponent; b++)
                {
                    if (components[b]->get_type() == "VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                    {
                        if (components[b]->get_node1() == 0)
                        {
                            Ys(components[b]->get_node2() - 1, components[b]->get_node2() - 1) += 1;
                            IS(components[b]->get_node2() - 1, 0) -= components[b]->get_voltageSource();
                        }
                        else
                        {
                            Ys(components[b]->get_node1() - 1, components[b]->get_node1() - 1) += 1;
                            IS(components[b]->get_node1() - 1, 0) += components[b]->get_voltageSource();
                        }
                    }
                    //stop here
                    if (components[b]->get_type() == "Dep_VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                    {
                        if (components[b]->get_node1() == 0)
                        {
                            Ys(components[b]->get_node2() - 1, components[b]->get_node2() - 1) -= 1;

                            if (components[b]->getnode1_depend() == 0)
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode2_depend() - 1) += components[b]->getScaler_depend();
                            }

                            else if (components[b]->getnode2_depend() == 0)
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode1_depend() - 1) -= components[b]->getScaler_depend();
                            }
                            else
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode2_depend() - 1) += components[b]->getScaler_depend();
                                Ys(components[b]->get_node2() - 1, components[b]->getnode1_depend() - 1) -= components[b]->getScaler_depend();
                            }
                        }
                        else
                        {
                            Ys(components[b]->get_node2() - 1, components[b]->get_node2() - 1) += 1;

                            if (components[b]->getnode1_depend() == 0)
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode2_depend() - 1) += components[b]->getScaler_depend();
                            }

                            else if (components[b]->getnode2_depend() == 0)
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode1_depend() - 1) -= components[b]->getScaler_depend();
                            }
                            else
                            {
                                Ys(components[b]->get_node2() - 1, components[b]->getnode2_depend() - 1) += components[b]->getScaler_depend();
                                Ys(components[b]->get_node2() - 1, components[b]->getnode1_depend() - 1) -= components[b]->getScaler_depend();
                            }
                        }
                    }
                }
            }
            if (is_super == true)
            {
                int positionVoltage=60;
               
                /////////////////////////////////////////////////////////////////////////// 
                for (int b = 0; b < CountComponent; b++)
                    if (components[b]->get_type() == "VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                    {
                        positionVoltage = b;
                    }
                if (positionVoltage != 60)
                {
                    if (node1_super == i + 1)
                    {
                        Ys(node2_super - 1, node2_super - 1) -= 1;
                        Ys(node2_super - 1, node1_super - 1) += 1;
                        IS(node2_super - 1, 0) += components[positionVoltage]->get_voltageSource();
                        ignore[node2_super - 1] = true;
                    }
                    else
                    {
                        Ys(node1_super - 1, node1_super - 1) += 1;
                        Ys(node1_super - 1, node2_super - 1) -= 1;
                        IS(node1_super - 1, 0) += components[positionVoltage]->get_voltageSource();
                        ignore[node1_super - 1] = true;
                    }
                }
                ///////////////////////////////////////////////////////////////////////////////
                for (int b = 0; b < CountComponent; b++)
                    if (components[b]->get_type() == "Dep_VoltageSource" && (components[b]->get_node1() == i + 1 || components[b]->get_node2() == i + 1))
                    {
                        positionVoltage = b;
                    }
                if (components[positionVoltage]->get_type() == "Dep_VoltageSource")
                {
                    if (node1_super == i + 1)
                    {
                        Ys(node2_super - 1, node2_super - 1) -= 1;
                        Ys(node2_super - 1, node1_super - 1) += 1;

                        if (components[positionVoltage]->getnode1_depend() == 0)
                        {
                            Ys(node2_super - 1, components[positionVoltage]->getnode2_depend() - 1) += components[positionVoltage]->getScaler_depend();
                        }

                        else if (components[positionVoltage]->getnode2_depend() == 0)
                        {
                            Ys(node2_super - 1, components[positionVoltage]->getnode1_depend() - 1) -= components[positionVoltage]->getScaler_depend();
                        }
                        else
                        {
                            Ys(node2_super - 1, components[positionVoltage]->getnode2_depend() - 1) += components[positionVoltage]->getScaler_depend();
                            Ys(node2_super - 1, components[positionVoltage]->getnode1_depend() - 1) -= components[positionVoltage]->getScaler_depend();
                        }

                        ignore[node2_super - 1] = true;
                    }
                    else
                    {
                        Ys(node1_super - 1, node1_super - 1) += 1;
                        Ys(node1_super - 1, node2_super - 1) -= 1;

                        if (components[positionVoltage]->getnode1_depend() == 0)
                        {
                            Ys(node1_super - 1, components[positionVoltage]->getnode2_depend() - 1) += components[positionVoltage]->getScaler_depend();
                        }

                        else if (components[positionVoltage]->getnode2_depend() == 0)
                        {
                            Ys(node1_super - 1, components[positionVoltage]->getnode1_depend() - 1) -= components[positionVoltage]->getScaler_depend();
                        }
                        else
                        {
                            Ys(node1_super - 1, components[positionVoltage]->getnode2_depend() - 1) += components[positionVoltage]->getScaler_depend();
                            Ys(node1_super - 1, components[positionVoltage]->getnode1_depend() - 1) -= components[positionVoltage]->getScaler_depend();
                        }

                        ignore[node1_super - 1] = true;
                    }
                }
                /////////////////////////////////////////////////////////////////////////////
                int firstnode; int secondnode;
                if (node1_super == i + 1)
                {
                    firstnode = node1_super;
                    secondnode = node2_super;
                }
                else
                {
                    firstnode = node2_super;
                    secondnode = node1_super;
                }
                for (int z = 0; z < CountComponent; z++)
                {
                    if (components[z]->get_node1() == firstnode || components[z]->get_node2() == firstnode)
                    {
                        if (components[z]->get_type() == "CurrentSource")
                        {
                            if (components[z]->get_node1() == i + 1)
                            {
                                IS(firstnode - 1, 0) += components[z]->get_currentSource();
                            }
                            else
                            {
                                IS(firstnode - 1, 0) -= components[z]->get_currentSource();
                            }
                        }
                        //////////////////////////////....//////////////////////////
                        if (components[z]->get_type() == "Dep_CurrentSource")
                        {
                            int position_of_dependent;
                            for (int h = 0; h < CountComponent; h++)
                            {
                                if (components[h]->getname() == components[z]->getname_Depend())
                                {
                                    position_of_dependent = h;
                                }
                            }
                            if (components[z]->getnode1_depend() != 0 && components[z]->getnode2_depend() != 0)
                            {
                                if (components[z]->get_node1() == i + 1)
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                            if (components[z]->getnode1_depend() == 0)
                            {
                                if (components[z]->get_node1() == i + 1)
                                {
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode- 1, components[z]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                            if (components[z]->getnode2_depend() == 0)
                            {
                                if (components[z]->get_node1() == i + 1)
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                        }

                        //////////////////////////////.....////////////////////////// 
                        if (components[z]->get_type() == "resistance" || components[z]->get_type() == "capacitor" || components[z]->get_type() == "inductor")
                        {
                            if (components[z]->get_node1() == i + 1)
                            {
                                if (components[z]->get_node2() == 0)
                                {
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) += components[z]->get_y();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) += components[z]->get_y();
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) -= components[z]->get_y();
                                }
                            }

                            if (components[z]->get_node2() == i + 1)
                            {
                                if (components[z]->get_node1() == 0)
                                {
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) += components[z]->get_y();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) += components[z]->get_y();
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) -= components[z]->get_y();
                                }
                            }
                        }
                    }
                    if (components[z]->get_node1() == secondnode || components[z]->get_node2() == secondnode)
                    {
                        if (components[z]->get_type() == "CurrentSource")
                        {
                            if (components[z]->get_node1() == secondnode)
                            {
                                IS(firstnode - 1, 0) += components[z]->get_currentSource();
                            }
                            else
                            {
                                IS(firstnode - 1, 0) -= components[z]->get_currentSource();
                            }
                        }
                        
                        /// /////////////////////////////////////////////////////////////////////////
                        if (components[z]->get_type() == "Dep_CurrentSource")
                        {
                            int position_of_dependent;
                            for (int h = 0; h < CountComponent; h++)
                            {
                                if (components[h]->getname() == components[z]->getname_Depend())
                                {
                                    position_of_dependent = h;
                                }
                            }
                            if (components[z]->getnode1_depend() != 0 && components[z]->getnode2_depend() != 0)
                            {
                                if (components[z]->get_node1() == secondnode)
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                            if (components[z]->getnode1_depend() == 0)
                            {
                                if (components[z]->get_node1() == i + 1)
                                {
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->getnode2_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                            if (components[z]->getnode2_depend() == 0)
                            {
                                if (components[z]->get_node1() == i + 1)
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) += components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->getnode1_depend() - 1) -= components[position_of_dependent]->get_y() * components[z]->getScaler_depend();
                                }
                            }
                        }
                        /////////////////////////////////////////////////////////////////////////////
                        if (components[z]->get_type() == "resistance" || components[z]->get_type() == "capacitor" || components[z]->get_type() == "inductor")
                        {
                            if (components[z]->get_node1() == secondnode)
                            {
                                if (components[z]->get_node2() == 0)
                                {
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) += components[z]->get_y();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) += components[z]->get_y();
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) -= components[z]->get_y();
                                }
                            }

                            if (components[z]->get_node2() == secondnode)
                            {
                                if (components[z]->get_node1() == 0)
                                {
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) += components[z]->get_y();
                                }
                                else
                                {
                                    Ys(firstnode - 1, components[z]->get_node2() - 1) += components[z]->get_y();
                                    Ys(firstnode - 1, components[z]->get_node1() - 1) -= components[z]->get_y();
                                }
                            }
                        }
                    }

                }
            }

        }
    }
    
    cout << "\n\n\n\n";
    VS = Ys.inverse() * IS;
    cout << IS << "\n\n\n";
    cout << Ys << "\n\n\n";
    //cout << IS;
    cout << "\n\n\n\n\n";
     cout << VS;
     ofstream output("output.txt");
     output.clear();
     for (int i = 0; i < nodes_num; i++)
     {
         output << VS(i, 0) << endl;
     }
     output.close();
}
