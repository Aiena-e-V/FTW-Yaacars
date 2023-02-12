--________________________________________________________--
-- FlyWithLua AddOn-Script for YAACARS / FTW
-- This script will load the plane with fuel and payload
-- exported by the yaacars ACARS client
--
-- This script is a demo only - you have to adapt it
-- to your own needs!
--
-- Feel free to return any changes back to me, so we can
-- release it to the FTW community.
--________________________________________________________--
--History:
--
--Version 1.0 2019-01-06 Teddii
--	release
--________________________________________________________--

-- this is what I use ... you have to adapt the path! Make sure to use / instead of \ !!!
gYaacarsPath="g:/spiele/X-Plane/tools/YAACARS/WORK/build-YAACARS-Desktop_Qt_5_11_2_MinGW_32bit-Release"
gYaacarsPath="C:/Users/mk/Desktop/YAACARS/win32"

planeMatch = {}
-- this list will map your planes in X-Plane to FTW!
-- you can edit thePLANE_ICAO with Planemaker or change it in the plane's ACF file: P acf/_ICAO XXXX
----------- FTW Plane Name --------------------- -ICAO-
planeMatch['Cessna Citation II']               = "C550"
planeMatch['Cessna 208 Grand Caravan']         = "C208"
planeMatch['Cessna 208 Grand Caravan']         = "C208"
planeMatch['Diamond DA-42 Twin Star']          = "DA42"
planeMatch['DeHavilland DHC-6 300 Twin Otter'] = "DHC"
planeMatch['Cessna T210M Centurion II']        = "T210"
planeMatch['Piper PA-31-350 Navajo Chieftain'] = "PA31"

------------------------------------------------------------

dr_tank_ratio = dataref_table("sim/aircraft/overflow/acf_tank_rat")
dr_tank_kg = dataref_table("sim/flightmodel/weight/m_fuel")

dataref("dr_payload", "sim/flightmodel/weight/m_fixed", "writable")

dofile(gYaacarsPath.."/yaacars.lua") 	-- import the yaacars data table
-- just dump the numbers to X-Plane's log ...
logMsg("YAACARS-LM: tracking: "..YAACARS['tracking'])
logMsg("YAACARS-LM: plane:    "..YAACARS['plane_name'])
logMsg("YAACARS-LM: fuel:     "..YAACARS['fuel_kg'])
logMsg("YAACARS-LM: payload:  "..YAACARS['payload_kg'])
										
if (YAACARS['tracking'] == 0) -- we don't want to apply values when tracking runs!
then
	logMsg("YAACARS-LM: tracking not running, looking up for the plane icao ...")
	if (planeMatch[YAACARS['plane_name']] == PLANE_ICAO) -- lookup for a matching PLANE_ICAO in "planeMatch"
	then
		logMsg("YAACARS-LM: plane match found ... loading now ...")
		for tankID = 0, 8 do
			dr_tank_kg[tankID]=dr_tank_ratio[tankID]*YAACARS['fuel_kg']
		end

		dr_payload=YAACARS['payload_kg']
		
		text="Your plane is now loaded with "..YAACARS['fuel_kg'].." kilograms of fuel and "..YAACARS['payload_kg'].." kilograms of cargo! Have a safe flight!"
		--XPLMSpeakString(text)
		--bubble(100,100, "YAACARS-LoadManager", text)
		logMsg("YAACARS-LM: "..text)
	else
		logMsg("YAACARS-LM: Unknown plane icao '"..PLANE_ICAO.."'! Please add a 'planeMatch' entry!")
	end
else
	logMsg("YAACARS-LM: tracking running - won't to anything right now!")
end
logMsg("YAACARS-LM: all done! Have an ice day!")


