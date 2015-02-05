
BeginPackage["MapLookup`", {"Errors`"}];

lookup::usage = "";
mapContains::usage = "";
mapReplace::usage = "";
mapReplaceAdd::usage = "";
mapValueMap::usage = "";
lookupDefault::usage = "";
mapValueMapMultiple::usage = "";
mapAdd::usage = "";
mapEnsureKey::usage = "";
mapQuery::usage = "";
mapRefAdd::usage = "";
mapRefSet::usage = "";
mapRefAppend::usage = "";

Begin["`Private`"];

(* -------------------------------------------------------------------------- 
   Map lookup
   -------------------------------------------------------------------------- *)

VerifyRule[r_] :=
  If[! Head[r] === Rule,
    ThrowError["Expecting a rule, but found"<>ToString[r,InputForm]]];

VerifyMap[m_] :=
  Module[{},
    If[!ListQ[m],
      ThrowError["Expecting a map (list of rules) but found", ToString[m,InputForm]]];
    Map[VerifyRule, m]];

DefFn[
  lookup[map_List, key_Symbol, default_] :=
  lookupDefault[map, key, default]];

lookupRaw[map_List, key:(_Symbol|_String)] :=
  Module[{values},
    VerifyMap[map];

    values = Select[map, First[#] === key &];
    If[values == {},
       ThrowError["lookup failure: key " <> ToString[key] <> " not found in map " <> ToString[map,InputForm]]]; (* TODO: shorten this output *)
    If[Length[values] > 1,
       ThrowError["lookup failure: key " <> ToString[key]<> " found multiple times in map ", ToString[map,InputForm]]];

    First[values][[2]]];

DefFn[lookup[map_List, key:(_Symbol|_String)] := 
 mapRefObarray[lookupRaw[map,key]]];

mapContains[map_, key_] :=
  Module[{},
    VerifyMap[map];
    Length[Select[map, First[#] === key &]] >= 1];

lookupDefault[map_, key_, default_] :=
  Module[{},
  VerifyMap[map];
    If[mapContains[map, key],
      lookup[map, key],
      default]];

mapReplace[map_, key_, value_] :=
  Module[{},
    VerifyMap[map];
    Map[If[First[#] === key,
           Module[{OAKeyStr=Last[#]},
             If[ mapRefObarraySetCount[OAKeyStr] < 0,
                 key -> value,
                 mapRefObarraySetCount[OAKeyStr] =
                    mapRefObarraySetCount[OAKeyStr] + 1;
                 mapRefObarray[OAKeyStr] = value;
                 # ]],
           #] &, map]];

mapReplaceAdd[map_, key_, value_] :=
  Module[{},
    VerifyMap[map];
  If[mapContains[map, key],
     mapReplace[map, key, value],
     mapAdd[map, key, value]]];

mapAdd[map_, key_, value_] :=
  Module[{},
    VerifyMap[map];
    Join[map, {key -> value}]];

mapEnsureKey[map_, key_, defaultValue_] :=
  Module[{},
    VerifyMap[map];
  If[mapContains[map, key],
     map,
     mapAdd[map, key, defaultValue]]];

mapValueMap[map_, key_, f_] :=
  If[mapContains[map, key],
     mapReplace[map, key, f[lookup[map,key]]],
     map];

(* defs are rules of the form key -> function.  m is a map.  The result is
   a version of m which the functions applied to the corresponding keys
   at the top level only. This function needs to be renamed! *)
mapValueMapMultiple[m_, defs_] :=
  Fold[mapValueMap[#1, #2[[1]], #2[[2]]] &,
       m, defs];

mapQuery[ms_, key_, value_] :=
  Select[Select[ms, mapContains[#, key] &], lookup[#, key] === value &];

mapRefObarray[x_] := x; 
mapRefObarraySetCount[___] = -1;

mapRefAdd[map_List,key:(_Symbol|_String)]:=
   Module[{mapRefObarrayKey, OAKeyStr},
     VerifyMap[map];
     If[ mapContains[map,key], 
         ThrowError["Map already contains "<>ToString[key]]];
     OAKeyStr = ToString[mapRefObarrayKey];
     mapRefObarraySetCount[OAKeyStr] = 0;
     mapRefObarray[OAKeyStr] := 
        Throw["Value of mapRef key "
                 <> ToString[key] <>" accessed before being set."];
     mapAdd[map,key,OAKeyStr]];

mapRefAdd[map_List,keys_List] := Fold[mapRefAdd,map,keys];

mapRefPrepare[map_List,key:(_Symbol|_String)]:=
   Module[{OAKeyStr},
     VerifyMap[map];
     OAKeyStr = lookupRaw[map,key];
     If[ mapRefObarraySetCount[OAKeyStr] < 0,
         Throw["mapRefSet called with a non-reference key \""<>
                  ToString[key]<>"\". Use mapRefAdd."] ];
     mapRefObarraySetCount[OAKeyStr] = mapRefObarraySetCount[OAKeyStr] + 1;
     OAKeyStr];

mapRefSet[map_List,key:(_Symbol|_String), value_]:=
   Module[{OAKeyStr},
     VerifyMap[map];
     OAKeyStr = lookupRaw[map,key];
     If[ mapRefObarraySetCount[OAKeyStr] < 0,
         Throw["mapRefSet called with a non-reference key \""<>
                  ToString[key]<>"\". Use mapRefAdd."] ];
     mapRefObarraySetCount[OAKeyStr] = mapRefObarraySetCount[OAKeyStr] + 1;
     mapRefObarray[OAKeyStr] = value];

mapRefAppend[map_List,key:(_Symbol|_String), value_String]:=
   Module[{OAKeyStr,prevVal},
     OAKeyStr = mapRefPrepare[map,key];
     prevVal = If[ mapRefObarraySetCount[OAKeyStr] === 1, "",
                   mapRefObarray[OAKeyStr] ];
     mapRefObarray[OAKeyStr] = prevVal <> value];

End[];


EndPackage[];

