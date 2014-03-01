
(* Mathematica Test File *)

$derivatives = {
  PDstandard2nd[i_]     -> StandardCenteredDifferenceOperator[1,1,i],
  PDstandard2nd[i_, i_] -> StandardCenteredDifferenceOperator[2,1,i]};

PD = PDstandard2nd;

$groups = {{"evolved_group", {phi, pi}}};

$initialSineCalc = {
  Name      -> "initial_sine",
  Schedule  -> {"AT initial"},
  Equations ->
  {
    phi -> Sin[2 Pi (x - t)],
    pi  -> -2 Pi Cos[2 Pi (x - t)]
  }};

$evolveCalc = {
  Name      -> "calc_rhs",
  Schedule  -> {"IN MoL_CalcRHS"},
  Where     -> Interior,
  Equations ->
  {
    dot[phi] -> pi,
    dot[pi]  -> Euc[ui,uj] PD[phi,li,lj]
  }};



Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    Append[$groups,{"ode_group", {a, b}}],
    "TestThorns", "SimpleWaveODE",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    ODEGroups -> {"ode_group"},
    Calculations       -> {
      $initialSineCalc /. ((Equations -> l) :>
                           (Equations -> Join[l, {a->0,b->1}])),
      $evolveCalc /. ((Equations -> l) :>
                      (Equations -> Join[l, {dot[a] -> b, dot[b] -> -a}]))}]
  ,
  Null
  ,
  TestID->"SimpleWaveODE"
]


(****************************************************************)
(* CreateThorn *)
(****************************************************************)

Test[
        ClearAllTensors[];
	CreateKrancThornTT[{}, "TestThorns", "CreateThorn", Calculations -> {}]
	,
	Null
	,
	TestID->"CreateThorn"
]

(****************************************************************)
(* IfThen *)
(****************************************************************)

(* Test that IfThen statements are only factored out into if
   statements if they don't contain grid functions, as this doesn't
   work with vectorisation. *)

(* The PD symbol is treated in a nonoptimal way by Kranc, and this
   causes problems for the tests.  This should be cleaned up. *)

Test[
  Module[
    {derivatives = {
      PDstandard2nd[i_]     -> StandardCenteredDifferenceOperator[1,1,i],
      PDstandard2nd[i_, i_] -> StandardCenteredDifferenceOperator[2,1,i]},
     groups = {{"evolved_group", {phi, pi}}},
     initialSineCalc = {
       Name      -> "initial_sine",
       Schedule  -> {"AT initial"},
       Equations ->
       {
         phi -> IfThen[x > 0, Sin[2 Pi (x - t)], 0],
         pi  -> IfThen[x > 0, -2 Pi Cos[2 Pi (x - t)], 0]
       }},
     evolveCalc = {
       Name      -> "calc_rhs",
       Schedule  -> {"IN MoL_CalcRHS"},
       Where     -> Interior,
       Equations ->
       {
         dot[phi] -> IfThen[alpha>0, pi, 2 pi],
         dot[pi]  -> IfThen[alpha>0, Euc[ui,uj] PD[phi,li,lj], 2 Euc[ui,uj] PD[phi,li,lj]]
       }}},

     ClearAllTensors[];
    (* PD = PDstandard2nd *)
    
    CreateKrancThornTT[
      groups, "TestThorns", 
      "IfThen", 
      PartialDerivatives -> derivatives,
      RealParameters     -> {alpha},
      DeclaredGroups     -> {"evolved_group"},
      Calculations       -> {initialSineCalc, evolveCalc}]];

  {StringMatchQ[Import["TestThorns/IfThen/src/calc_rhs.cc","Text"],
                 __~~"if (alpha > 0)"~~__],
   StringMatchQ[Import["TestThorns/IfThen/src/initial_sine.cc","Text"],
                __~~"IfThen(xL"~~Whitespace~~">"~~Whitespace~~"0"~~__]}
  ,
  {True, True}
  ,
  TestID -> "IfThen"
    ]

(****************************************************************)
(* SimpleWave *)
(****************************************************************)

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "SimpleWave",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, $evolveCalc}]
  ,
  Null
  ,
  TestID->"SimpleWave"
]

Test[
  Module[
    {eulerCons, evolvedGroups, nonevolvedGroups, declaredGroups, declaredGroupNames, groups},

    ClearAllTensors[];
    Map[DefineTensor, {Den, S, En, rho, v, p}];

    evolvedGroups = Map[CreateGroupFromTensor, {Den, S[uj], En}];
    nonevolvedGroups = Map[CreateGroupFromTensor, {rho, v[uj], p}];
    
    declaredGroups = Join[evolvedGroups, nonevolvedGroups];
    declaredGroupNames = Map[First, declaredGroups];
    
    groups = declaredGroups;

    eulerCons =
    {
      Name -> "eulerauto_cons_calc",
      Primitives -> {rho, v[ui], p},
      Equations ->
      {
        flux[Den,ui] -> rho v[ui],
        flux[S[uj],ui] -> rho v[ui] v[uj] + p Euc[ui,uj],
        flux[En,ui] -> v[ui](En + p)
      },
      ConservedEquations ->
      {
        Den -> rho,
        S[ui] -> rho v[ui],
        En -> p/(gamma-1) + 1/2 rho v[ui] v[uj] Euc[li,lj]
      },
      PrimitiveEquations ->
      {
        rho -> Den,
        v[ui] -> S[ui] / Den,
        p -> (gamma-1)(En - 1/2 Euc[li,lj] S[ui] S[uj]/Den)
      }
    };

    CreateKrancThornTT[
      groups, "TestThorns", "ConservationCalculation", 
      Calculations -> {},
      ConservationCalculations -> {eulerCons},
      RealParameters -> {gamma},
      DeclaredGroups -> declaredGroupNames]];
  ,
  Null
  ,
  TestID->"ConservationCalculation"];

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "Analysis",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc /.
                           ((Schedule -> l) :>
                            (Schedule -> {"at CCTK_ANALYSIS"}))}]
  ,
  Null
  ,
  TestID->"Analysis"
]

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    {{"evolved_group", {phi, pi}, Timelevels -> 3}}, "TestThorns", "Analysis-3TL",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc /.
                           ((Schedule -> l) :>
                            (Schedule -> {"at CCTK_ANALYSIS"}))}]
  ,
  Null
  ,
  TestID->"Analysis-3TL"
]

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "ParamCheck",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, $evolveCalc},
    RealParameters -> {aparam},
    ParameterConditions -> {{aparam == 0, "aparam must be == 0"}}]
  ,
  Null
  ,
  TestID->"ParamCheck"
]


Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "LoopControlNone",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, $evolveCalc},
    UseLoopControl     -> False]
  ,
  Null
  ,
  TestID->"LoopControlNone"
]

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "LoopControlAll",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, $evolveCalc},
    UseLoopControl     -> True]
  ,
  Null
  ,
  TestID->"LoopControlAll"
]

Test[
  ClearAllTensors[];
  CreateKrancThornTT[
    $groups, "TestThorns", "LoopControlOne",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, Append[$evolveCalc,UseLoopControl->False]},
    UseLoopControl     -> True]
  ,
  Null
  ,
  TestID->"LoopControlNone"
]


Test[
Module[{evolveCalc, pd},

  pd[u_,1,1] := (GFOffset[u,1,0,0] + GFOffset[u,-1,0,0] - 2 u)/dx^2;
  pd[u_,2,2] := (GFOffset[u,0,1,0] + GFOffset[u,0,-1,0] - 2 u)/dy^2;
  pd[u_,3,3] := (GFOffset[u,0,0,1] + GFOffset[u,0,0,-1] - 2 u)/dz^2;

  pdShort[u_,i_Integer,j_Integer] := Symbol["pd"<>ToString[u]<>ToString[i]<>ToString[j]];

  ClearAllTensors[];

  evolveCalc = {
  Name      -> "calc_rhs",
  Schedule  -> {"IN MoL_CalcRHS"},
  Where     -> Interior,
  Shorthands -> {pdphi11, pdphi22, pdphi33},
  Equations ->
  {
    pdphi11 -> pd[phi,1,1],
    pdphi22 -> pd[phi,2,2],
    pdphi33 -> pd[phi,3,3],

    dot[phi] -> pi,
    dot[pi]  -> Euc[ui,uj] pdShort[phi,li,lj]
  }};

  CreateKrancThornTT[
    $groups, "TestThorns", "GFOffset",
    PartialDerivatives -> $derivatives,
    DeclaredGroups     -> {"evolved_group"},
    Calculations       -> {$initialSineCalc, evolveCalc}]]
  ,
  Null
  ,
  TestID->"GFOffset"
]
