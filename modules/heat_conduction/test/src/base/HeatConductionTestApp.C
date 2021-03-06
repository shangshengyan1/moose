//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "HeatConductionTestApp.h"
#include "HeatConductionApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<HeatConductionTestApp>()
{
  InputParameters params = validParams<HeatConductionApp>();
  return params;
}

registerKnownLabel("HeatConductionTestApp");

HeatConductionTestApp::HeatConductionTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  HeatConductionApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  HeatConductionApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  HeatConductionApp::registerExecFlags(_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    HeatConductionTestApp::registerObjects(_factory);
    HeatConductionTestApp::associateSyntax(_syntax, _action_factory);
  }
}

HeatConductionTestApp::~HeatConductionTestApp() {}

// External entry point for dynamic application loading
extern "C" void
HeatConductionTestApp__registerApps()
{
  HeatConductionTestApp::registerApps();
}
void
HeatConductionTestApp::registerApps()
{
  registerApp(HeatConductionApp);
  registerApp(HeatConductionTestApp);
}

// External entry point for dynamic object registration
extern "C" void
HeatConductionTestApp__registerObjects(Factory & factory)
{
  HeatConductionTestApp::registerObjects(factory);
}
void
HeatConductionTestApp::registerObjects(Factory & factory)
{
  Registry::registerObjectsTo(factory, {"HeatConductionTestApp"});
}

// External entry point for dynamic syntax association
extern "C" void
HeatConductionTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  HeatConductionTestApp::associateSyntax(syntax, action_factory);
}
void
HeatConductionTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & action_factory)
{
  Registry::registerActionsTo(action_factory, {"HeatConductionTestApp"});
}

// External entry point for dynamic execute flag registration
extern "C" void
HeatConductionTestApp__registerExecFlags(Factory & factory)
{
  HeatConductionTestApp::registerExecFlags(factory);
}
void
HeatConductionTestApp::registerExecFlags(Factory & /*factory*/)
{
}
