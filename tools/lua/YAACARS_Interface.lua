--________________________________________________________--
-- FlyWithLua AddOn-Script for Fly the World, ftw-sim.de
--________________________________________________________--
--History:
--
--Version 1.0 2019-01-11 Teddii
--	release
--
--Version 1.1 2019-02-05 Teddii
--	warning is now triggered when running more than 10kts
--________________________________________________________--

--position of the interface
local XMin=50
local YMin=770

--________________________________________________________--
--________________________________________________________--
--________________________________________________________--

dr_tank_ratio = dataref_table("sim/aircraft/overflow/acf_tank_rat")
dr_tank_kg = dataref_table("sim/flightmodel/weight/m_fuel")
dataref("dr_payload", "sim/flightmodel/weight/m_fixed", "writable")

DataRef( "dr_yaacars_connected", 	"yaacars/connected")
DataRef( "dr_yaacars_tracking", 	"yaacars/tracking")
DataRef( "dr_yaacars_alive",		"yaacars/alive")
DataRef( "dr_yaacars_fuel_kg",		"yaacars/fuel_kg")
DataRef( "dr_yaacars_payload_kg",	"yaacars/payload_kg")

local yaacars_alive_clock=0
local yaacars_connected=0
local yaacars_tracking=0
local yaacars_alive_timeout=5

--________________________________________________________--

--set to "deactivate" if you want to see the interface only when hovering the mouse over it
--add_macro("Always show YAACARS-Interface when on ground", "gAlwaysShowYaacarsInterfaceOnGround=true", "gAlwaysShowYaacarsInterfaceOnGround=false",
--			"activate")

function eventTakeOffWithoutYaacarsFlightStarted()
	XPLMSpeakString("YAACARS tracking not started!")
	--command_once("sim/operation/pause_toggle")
end
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--
--________________________________________________________--

require "graphics"
--________________________________________________________--

dataref("datRAlt", 			"sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_pilot")
dataref("datGSpd", 			"sim/flightmodel/position/groundspeed")

--________________________________________________________--

local XMax=XMin+130
local YMax=YMin+80

local StartButtonTimer=os.clock()

--________________________________________________________--

do_often("yaacars_interface_check_often()")
do_every_draw("yaacars_interface_info()")
do_on_mouse_click("yaacars_interface_events()")
--________________________________________________________--

do_every_frame("yaacars_interface_check()")
local aliveDiff=os.clock()-dr_yaacars_alive
function yaacars_interface_check()
	yaacars_alive_clock=dr_yaacars_alive+aliveDiff
	
	if((dr_yaacars_connected~=0 and yaacars_connected==0)
	or (dr_yaacars_tracking~=0 and yaacars_tracking==0)) --connection changes or tracking starts
	then
		aliveDiff=os.clock()-dr_yaacars_alive
	end

	yaacars_connected=dr_yaacars_connected
	yaacars_tracking=dr_yaacars_tracking
end
--________________________________________________________--

local flightNotStarted=0
function yaacars_interface_check_often()
	GS=math.floor(datGSpd*1.94384449) --datGSpd is m/s (not kts)

	--if(datRAlt>25 and datGSpd>25)
	if(GS>10)
	then
		if(flightNotStarted<3) then --count from 0 to 3 (because we check the flight state more often as the FSE client does)
			flightNotStarted=flightNotStarted+1
		end
		if(flightNotStarted==2) then --trigger event once(!) on stage 2
			if (yaacars_tracking==0) then
				eventTakeOffWithoutYaacarsFlightStarted()
			end
		end
	end
	--if(datRAlt<20 and datGSpd<5) then
	if(GS<5) then
		flightNotStarted=0
	end
	--yaacars_connected=0
	--yaacars_tracking=0
end


local showInterface
function yaacars_interface_info()
	--stqDrawString(4, "YAACARS-Fuel: "..dr_yaacars_fuel_kg, "yellow")
	--stqDrawString(4, "YAACARS-PayL: "..dr_yaacars_payload_kg, "yellow")
	--stqDrawString(4, "YAACARS-Connd: "..yaacars_connected, "yellow")
	--stqDrawString(4, "YAACARS-Track: "..yaacars_tracking, "yellow")
	--stqDrawString(4, "YAACARS-Alive: "..dr_yaacars_alive, "yellow")
	--stqDrawString(4, "YAACARS-ADiff : "..aliveDiff, "yellow")
	--stqDrawString(4, "YAACARS-Clock: "..yaacars_alive_clock, "yellow")
	--stqDrawString(4, "System -Clock: "..os.clock(), "yellow")
	--stqDrawString(4, "System -Diff : "..(os.clock()-yaacars_alive_clock), "yellow")
	--if(math.abs(os.clock()-yaacars_alive_clock)<2) then
	--	stqDrawString(4, "ALIVE: yes", "green")
	--else
	--	stqDrawString(4, "ALIVE: NO", "red")
	--end

	showInterface=false
	-- does we have to draw anything?
	if MOUSE_X > XMin and MOUSE_X < XMax and MOUSE_Y > YMin and MOUSE_Y < YMax then
		showInterface=true
	end
	--if(datRAlt<10 and datGSpd<5) then
	--	showInterface=true
	--end
	if(yaacars_tracking==0) then
		showInterface=true
	elseif(math.abs(os.clock()-yaacars_alive_clock)>yaacars_alive_timeout) then
		showInterface=true
	end

	if(showInterface==false) then
		return
	end
	
	-- init the graphics system
	XPLMSetGraphicsState(0,0,0,1,1,0,0)

	-- draw transparent backgroud
	graphics.set_color(0, 0, 0, 0.5)
	graphics.draw_rectangle(XMin, YMin, XMax, YMax)

	--if(datOnGround~=0) then
	--	draw_string_Helvetica_10(XMin+5, YMin+85, "Plane is ON GROUND!")
	--end
	
	-- draw lines around the hole block
	if(yaacars_connected==0) then
		graphics.set_color(0.8, 0.8, 0.8, 0.5) 	 --grey
	else
		if(yaacars_tracking==0) then
			graphics.set_color(1, 0, 0, 0.5) 	 --red
		elseif(math.abs(os.clock()-yaacars_alive_clock)>yaacars_alive_timeout) then -- yaacars_tracking==1, but alive fails!
			graphics.set_color(1, 1, 0, 0.5) 	 --yellow
		else 
			graphics.set_color(0, 1, 0, 0.5)	 --green
		end
	end
	graphics.set_width(2)
	graphics.draw_line(XMin, YMin, XMin, YMax)
	graphics.draw_line(XMin, YMax, XMax, YMax)
	graphics.draw_line(XMax, YMax, XMax, YMin)
	graphics.draw_line(XMax, YMin, XMin, YMin)

	graphics.draw_line(XMin, 	YMin+30, XMax-1,  YMin+30) 	--hor
	graphics.draw_line(XMin+35, YMin+1,  XMin+35, YMin+30)	--vert1
	graphics.draw_line(XMin+40, YMin+1,  XMin+40, YMin+30)	--vert2
	graphics.draw_line(XMin+86, YMin+1,  XMin+86, YMin+30)	--vert3

	graphics.set_color(1, 1, 1, 0.8)

	if(yaacars_connected==0) then
			draw_string_Helvetica_10(XMin+5, YMin+67,     "Status                     offline")
	else
		if(yaacars_tracking==0) then
			draw_string_Helvetica_10(XMin+5, YMin+67,     "Status              connected")
		elseif (math.abs(os.clock()-yaacars_alive_clock)>yaacars_alive_timeout) then
			draw_string_Helvetica_10(XMin+5, YMin+67,     "Status              NO ALIVE")
		else
			draw_string_Helvetica_10(XMin+5, YMin+67,     "Status                 tracking")
		end
		
		if(yaacars_tracking==0) then
			str=string.format("Fuel (kg)                %6i",math.floor(dr_yaacars_fuel_kg))
			if(dr_yaacars_fuel_kg~=0) then
				draw_string_Helvetica_10(XMin+5, YMin+52, str)
			end
			str=string.format("Payload (kg)          %6i",math.floor(dr_yaacars_payload_kg))
			if(dr_yaacars_payload_kg~=0) then
				draw_string_Helvetica_10(XMin+5, YMin+37, str)
			end
		else
			str=string.format("Alive  Clock            %6i",yaacars_alive_clock)
			draw_string_Helvetica_10(XMin+5, YMin+52, str)
			str=string.format("System Clock         %6i",os.clock())
			draw_string_Helvetica_10(XMin+5, YMin+37, str)
		end
	end
	
	if(yaacars_connected==1) then
		if(yaacars_tracking==0) then
			if(dr_yaacars_fuel_kg~=0) then
				draw_string_Helvetica_10(XMin+44, YMin+17, "Load")
				draw_string_Helvetica_10(XMin+44, YMin+5,  "Fuel")
			end
			if(dr_yaacars_payload_kg~=0) then
				draw_string_Helvetica_10(XMin+91, YMin+17, "Load")
				draw_string_Helvetica_10(XMin+91, YMin+5,  "Payload")
			end
		end
	end

end
--________________________________________________________--

function yaacars_interface_events()
	-- we will only react once
	if MOUSE_STATUS ~= "down" then
		return
	end
	
	if MOUSE_X > XMin and MOUSE_X < XMin+36 and MOUSE_Y > YMin and MOUSE_Y < YMin+31 then
		--if(yaacars_tracking==0) then
		--dofile(gYaacarsPath.."/yaacars.lua") 	-- import the yaacars data table
		--end
		RESUME_MOUSE_CLICK = false
	end
	if MOUSE_X > XMin+50 and MOUSE_X < XMin+70 and MOUSE_Y > YMin+5 and MOUSE_Y < YMin+25 then
		if(yaacars_tracking==0 and dr_yaacars_fuel_kg~=0) then
			for tankID = 0, 8 do
				dr_tank_kg[tankID]=dr_tank_ratio[tankID]*dr_yaacars_fuel_kg;
			end
		end
		RESUME_MOUSE_CLICK = false
	end
	if MOUSE_X > XMin+90 and MOUSE_X < XMin+120 and MOUSE_Y > YMin+5 and MOUSE_Y < YMin+25 then
		if(yaacars_tracking==0 and dr_yaacars_payload_kg~=0) then
			dr_payload=dr_yaacars_payload_kg
		end
		RESUME_MOUSE_CLICK = false
	end
	--if MOUSE_X > XMin+5 and MOUSE_X < XMax-5 and MOUSE_Y > YMin+35 and MOUSE_Y < YMax-5 then
	--	--command_once("fse/window/toggle")
	--	RESUME_MOUSE_CLICK = false
	--end
end
--________________________________________________________--
