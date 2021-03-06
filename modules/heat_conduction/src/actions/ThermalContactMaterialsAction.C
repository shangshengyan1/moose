//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ThermalContactMaterialsAction.h"
#include "ThermalContactAuxVarsAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseApp.h"
#include "Conversion.h"
#include "AddVariableAction.h"
#include "GapConductance.h"

registerMooseAction("HeatConductionApp", ThermalContactMaterialsAction, "add_material");

template <>
InputParameters
validParams<ThermalContactMaterialsAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::string>(
      "type",
      "A string representing the Moose object that will be used for heat conduction over the gap");

  params.addRequiredParam<NonlinearVariableName>("variable", "The variable for thermal contact");
  params.addRequiredParam<BoundaryName>("master", "The master surface");
  params.addRequiredParam<BoundaryName>("slave", "The slave surface");
  params.addParam<Real>("gap_conductivity", 1.0, "The thermal conductivity of the gap material");
  params.addParam<FunctionName>(
      "gap_conductivity_function",
      "Thermal conductivity of the gap material as a function.  Multiplied by gap_conductivity.");
  params.addParam<std::vector<VariableName>>(
      "gap_conductivity_function_variable",
      "Variable to be used in gap_conductivity_function in place of time");

  params.addParam<bool>(
      "quadrature", false, "Whether or not to use quadrature point based gap heat transfer");

  params.addParam<std::string>("conductivity_name",
                               "thermal_conductivity",
                               "The name of the MaterialProperty associated with conductivity "
                               "(\"thermal_conductivity\" in the case of heat conduction)");
  params.addParam<std::string>("conductivity_master_name",
                               "thermal_conductivity",
                               "The name of the MaterialProperty associated with conductivity "
                               "(\"thermal_conductivity\" in the case of heat conduction)");

  params += GapConductance::actionParameters();
  return params;
}

ThermalContactMaterialsAction::ThermalContactMaterialsAction(const InputParameters & params)
  : Action(params)
{
}

void
ThermalContactMaterialsAction::act()
{
  if (getParam<std::string>("type") != "GapHeatTransfer")
    return;

  const std::string type = "GapConductance";

  InputParameters params = _factory.getValidParams(type);
  params.applyParameters(parameters(), {"variable"});

  params.set<std::vector<VariableName>>("variable") = {getParam<NonlinearVariableName>("variable")};

  const bool quadrature = getParam<bool>("quadrature");
  if (!quadrature)
  {
    params.set<std::vector<VariableName>>("gap_temp") = {
        ThermalContactAuxVarsAction::getGapValueName(_pars)};
    params.set<std::vector<VariableName>>("gap_distance") = {"penetration"};
  }
  else
  {
    params.set<bool>("quadrature") = true;
    params.set<BoundaryName>("paired_boundary") = getParam<BoundaryName>("master");
  }

  params.set<std::vector<BoundaryName>>("boundary") = {getParam<BoundaryName>("slave")};

  std::string material_name = name() + "_" + "gap_value";
  _problem->addMaterial(type, material_name, params);

  if (quadrature)
  {
    params.set<BoundaryName>("paired_boundary") = getParam<BoundaryName>("slave");
    params.set<std::vector<BoundaryName>>("boundary") = {getParam<BoundaryName>("master")};
    params.set<std::string>("conductivity_name") =
        getParam<std::string>("conductivity_master_name");

    std::string master_name;
    master_name += name() + "_" + "gap_value_master";
    _problem->addMaterial(type, master_name, params);
  }
}
