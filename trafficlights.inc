/*
    TrafficLights v0.01 wrote by RIDE2DAY
    Official thread: http://forum.sa-mp.com/showthread.php?t=646429

    Many thanks to:
    	- Southclaws
    	- |stuntman|
    	- mschnitzer
*/

#if defined _trafficlights_included
	#endinput
#endif
#define _trafficlights_included


#include <YSI\y_hooks>
#include <YSI\y_iterate>


// ================================== [DEFINITIONS] ================================== //
#define INVALID_TL_ID					-1

#define MAX_TRAFFIC_LIGHTS_TYPES		8    		// Amount of different posts.
#define MAX_TRAFFIC_LIGHTS				50			// Max. amount of controllable traffic lights. Increase if you wish.


#if !defined TLC_GREEN
	#define TLC_GREEN					19283		// Green light model id.
#endif

#if !defined TLC_AMBER
	#define TLC_AMBER					-2063		// Amber light model id.
#endif

#if !defined TLC_RED
	#define TLC_RED						19282		// Red light model id.
#endif

#define TL_TYPE_1						-2055
#define TL_TYPE_2						-2056
#define TL_TYPE_3						-2057
#define TL_TYPE_4						-2058
#define TL_TYPE_5						-2059
#define TL_TYPE_6						-2060
#define TL_TYPE_7						-2061
#define TL_TYPE_8						-2062


// ================================== [VARIABLES] ================================== //
static bool:tl_script_loaded;
static bool:tl_script_unloaded;

enum E_TL_DATA
{
	tl_model_id,
	tl_object_id,
	tl_color,
	tl_light_object[3]
}
static TL_Data[MAX_TRAFFIC_LIGHTS][E_TL_DATA];
static Iterator:TL_Data<MAX_TRAFFIC_LIGHTS>;

// Data for each type of post (type, amount, offsets_idx_start, upper_light_offsets_idx).
static E_TL_TYPE_DATA[MAX_TRAFFIC_LIGHTS_TYPES][4] =
{
	{TL_TYPE_1, 2, 0, 0},
	{TL_TYPE_2, 1, 6, 0},
	{TL_TYPE_3, 2, 9, 15},
	{TL_TYPE_4, 2, 16, 0},
	{TL_TYPE_5, 2, 22, 0},
	{TL_TYPE_6, 1, 28, 31},
	{TL_TYPE_7, 2, 9, 15},
	{TL_TYPE_8, 2, 32, 0}
};

// Lights offsets for each type of post.
static Float:E_TL_OFFSETS[38][3] =
{
	// type 1
	{-0.630000, -0.299999, 2.649999}, 	// Green  	0
	{-1.020000, -0.299999, 2.649999}, 	// Amber  	1
 	{-1.410000, -0.299999, 2.649999}, 	// Red 		2
	{-7.300002, 0.310000, 2.649999}, 	// Green 	3    
	{-6.909999, 0.310000, 2.649999}, 	// Amber 	4
	{-6.509998, 0.310000, 2.649999}, 	// Red 		5

	// type 2
	{-0.270000, -0.009999, 4.099997}, 	// Green  	6
	{-0.270000, -0.009999, 4.389998}, 	// Amber  	7
 	{-0.270000, -0.009999, 4.639998}, 	// Red 		8

 	// type 3, type 7
 	{0.019999, -0.099999, 2.499999}, 	// Green  	9
	{0.019999, -0.099999, 2.779999}, 	// Amber  	10
 	{0.019999, -0.099999, 3.019999}, 	// Red 		11
	{-3.249999, -0.099999, 5.099998}, 	// Green 	12    
	{-3.249999, -0.099999, 5.369999}, 	// Amber 	13
	{-3.249999, -0.099999, 5.610001}, 	// Red 		14
	{-2.099999, 0.129999, 7.580002},	// Upper light 15

	// type 4
	{-3.039999, -0.439999, 2.499999}, 	// Green  	16
	{-3.039999, -0.439999, 2.859999}, 	// Amber  	17
 	{-3.039999, -0.439999, 3.179999}, 	// Red 		18
	{-3.039999, 0.770000, 2.499999}, 	// Green 	19    
	{-3.039999, 0.770000, 2.859999}, 	// Amber 	20
	{-3.039999, 0.770000, 3.179999}, 	// Red 		21

	// type 5
	{-1.550000, -0.460000, 2.000000}, 	// Green  	22
	{-1.550000, -0.460000, 2.500000}, 	// Amber  	23
 	{-1.550000, -0.460000, 3.000000}, 	// Red 		24
	{-9.420001, -0.460000, 2.000000}, 	// Green 	25    
	{-9.420001, -0.460000, 2.500000}, 	// Amber 	26
	{-9.420001, -0.460000, 3.000000}, 	// Red 		27

	// type 6
	{0.089999, -0.250000, 2.480000}, 	// Green  	28
	{0.089999, -0.250000, 2.759999}, 	// Amber  	29
 	{0.089999, -0.250000, 2.999999}, 	// Red 		30
 	{-1.719999, 0.019999, 5.980001},	// Upper light 31

 	// type 8
 	{1.109999, -0.540000, 2.020001}, 	// Green  	32
	{1.109999, -0.540000, 2.520000}, 	// Amber  	33
 	{1.109999, -0.540000, 3.019999}, 	// Red 		34
 	{-11.040002, 0.959999, 2.020001}, 	// Green  	35
	{-11.040002, 0.959999, 2.520000}, 	// Amber  	36
 	{-11.040002, 0.959999, 3.019999} 	// Red 		37
};


// ================================== [HOOKED CALLBACKS] ================================== //
hook OnGameModeInit()
{
	if(!tl_script_loaded)
	{
		tl_script_loaded = true;
		LoadTrafficLightsModels();
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnFilterScriptInit()
{
	if(!tl_script_loaded)
	{
		tl_script_loaded = true;
		LoadTrafficLightsModels();
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnGameModeExit()
{
	if(!tl_script_unloaded)
	{
		tl_script_unloaded = true;
		RemoveAllTrafficLights();
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnFilterScriptExit()
{
	if(!tl_script_unloaded)
	{
		tl_script_unloaded = true;
		RemoveAllTrafficLights();
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

// ================================== [NEW FUNCTIONS] ================================== //
stock CreateTrafficLight(type, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, init_color = TLC_GREEN)
{
	if(!IsValidTrafficLightPost(type))
	{
		print("** [TrafficLights] The traffic light couldn't be created, the post model isn't correct.");
		return INVALID_TL_ID;
	}

	if(!IsValidTrafficLightColor(init_color))
	{
		print("** [TrafficLights] The traffic light couldn't be created, the color isn't correct.");
		return INVALID_TL_ID;
	}

	new idx = Iter_Alloc(TL_Data);

	if(idx == INVALID_ITERATOR_SLOT)
	{
		print("** [TrafficLights] The traffic light couldn't be created, the limit has been reached.");
		return INVALID_TL_ID;
	}

	TL_Data[idx][tl_model_id] = type;
	TL_Data[idx][tl_color] = init_color;

	#if defined TL_USE_STREAMER

		TL_Data[idx][tl_object_id] = CreateDynamicObject(type, x, y, z, rx, ry, rz);

	#else

		TL_Data[idx][tl_object_id] = CreateObject(type, x, y, z, rx, ry, rz);

	#endif

	new upper_light = CheckPostUpperLight(type);

	if(upper_light)
	{
		#if defined TL_USE_STREAMER

			TL_Data[idx][tl_light_object][2] = CreateDynamicObject(19281, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachDynamicObjectToObject(TL_Data[idx][tl_light_object][2], TL_Data[idx][tl_object_id], E_TL_OFFSETS[upper_light][0], E_TL_OFFSETS[upper_light][1], E_TL_OFFSETS[upper_light][2], 0.0, 0.0, 0.0, 0);

		#else

			TL_Data[idx][tl_light_object][2] = CreateObject(19281, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachObjectToObject(TL_Data[idx][tl_light_object][2], TL_Data[idx][tl_object_id], E_TL_OFFSETS[upper_light][0], E_TL_OFFSETS[upper_light][1], E_TL_OFFSETS[upper_light][2], 0.0, 0.0, 0.0, 0);

		#endif
	}

	SetTrafficLightColor(idx, init_color);

	return idx;
}

stock SetTrafficLightColor(idx, color)
{
	if(!Iter_Contains(TL_Data, idx))
	{
		return 0;
	}

	if(!IsValidTrafficLightColor(color))
	{
		print("** [TrafficLights] The light couldn't be set, the color isn't correct.");
		return INVALID_TL_ID;
	}

	new offsets_idx = GetPostOffsetsIndex(TL_Data[idx][tl_model_id]);
	new lights_amount = GetPostRequiredLights(TL_Data[idx][tl_model_id]);

	switch(color)
	{
		case TLC_AMBER: offsets_idx += 1;
		case TLC_RED: offsets_idx += 2;
	}

	DisableTrafficLight(idx);

	for(new x = 0; x < lights_amount; x++)
	{
		#if defined TL_USE_STREAMER

			TL_Data[idx][tl_light_object][x] = CreateDynamicObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachDynamicObjectToObject(TL_Data[idx][tl_light_object][x], TL_Data[idx][tl_object_id], E_TL_OFFSETS[offsets_idx][0], E_TL_OFFSETS[offsets_idx][1], E_TL_OFFSETS[offsets_idx][2], 0.0, 0.0, 0.0, 0);

		#else

			TL_Data[idx][tl_light_object][x] = CreateObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachObjectToObject(TL_Data[idx][tl_light_object][x], TL_Data[idx][tl_object_id], E_TL_OFFSETS[offsets_idx][0], E_TL_OFFSETS[offsets_idx][1], E_TL_OFFSETS[offsets_idx][2], 0.0, 0.0, 0.0, 0);

		#endif

		offsets_idx += 3;
	}
	return 1;
}

stock GetTrafficLightColor(idx)
{
	if(!Iter_Contains(TL_Data, idx))
	{
		return 0;
	}
	return TL_Data[idx][tl_color];
}

stock DisableTrafficLight(idx)
{
	if(!Iter_Contains(TL_Data, idx))
	{
		return 0;
	}

	new lights_amount = GetPostRequiredLights(TL_Data[idx][tl_model_id]);

	for(new x = 0; x < lights_amount; x++)
	{
		#if defined TL_USE_STREAMER

			if(IsValidDynamicObject(TL_Data[idx][tl_light_object][x]))
			{
				DestroyDynamicObject(TL_Data[idx][tl_light_object][x]);
			}

		#else

			if(IsValidObject(TL_Data[idx][tl_light_object][x]))
			{
				DestroyObject(TL_Data[idx][tl_light_object][x]);
			}

		#endif
	}

	return 1;
}

stock RemoveTrafficLight(idx)
{
	if(!Iter_Contains(TL_Data, idx))
	{
		return 0;
	}

	DisableTrafficLight(idx);

	#if defined TL_USE_STREAMER

		if(IsValidDynamicObject(TL_Data[idx][tl_light_object][2]))
		{
			DestroyDynamicObject(TL_Data[idx][tl_light_object][2]);
		}

		if(IsValidDynamicObject(TL_Data[idx][tl_object_id]))
		{
			DestroyDynamicObject(TL_Data[idx][tl_object_id]);
		}

	#else

		if(IsValidObject(TL_Data[idx][tl_light_object][2]))
		{
			DestroyObject(TL_Data[idx][tl_light_object][2]);
		}

		if(IsValidObject(TL_Data[idx][tl_object_id]))
		{
			DestroyObject(TL_Data[idx][tl_object_id]);
		}

	#endif

	Iter_Remove(TL_Data, idx);
	return 1;
}

stock RemoveAllTrafficLights()
{
	foreach(new x : TL_Data)
	{
		DisableTrafficLight(x);

		#if defined TL_USE_STREAMER

			if(IsValidDynamicObject(TL_Data[x][tl_light_object][2]))
			{
				DestroyDynamicObject(TL_Data[x][tl_light_object][2]);
			}

			if(IsValidDynamicObject(TL_Data[x][tl_object_id]))
			{
				DestroyDynamicObject(TL_Data[x][tl_object_id]);
			}

		#else

			if(IsValidObject(TL_Data[x][tl_light_object][2]))
			{
				DestroyObject(TL_Data[x][tl_light_object][2]);
			}

			if(IsValidObject(TL_Data[x][tl_object_id]))
			{
				DestroyObject(TL_Data[x][tl_object_id]);
			}

		#endif
	}

	Iter_Clear(TL_Data);
	return 1;
}

stock GetPostRequiredLights(tl_type)
{
	for(new x = 0; x < MAX_TRAFFIC_LIGHTS_TYPES; x++)
	{
		if(E_TL_TYPE_DATA[x][0] == tl_type)
		{
			return E_TL_TYPE_DATA[x][1];
		}
	}
	return 0;
}

stock GetPostOffsetsIndex(tl_type)
{
	for(new x = 0; x < MAX_TRAFFIC_LIGHTS_TYPES; x++)
	{
		if(E_TL_TYPE_DATA[x][0] == tl_type)
		{
			return E_TL_TYPE_DATA[x][2];
		}
	}
	return 0;
}

stock CheckPostUpperLight(tl_type)
{
	for(new x = 0; x < MAX_TRAFFIC_LIGHTS_TYPES; x++)
	{
		if(E_TL_TYPE_DATA[x][0] == tl_type && E_TL_TYPE_DATA[x][3] != 0)
		{
			return E_TL_TYPE_DATA[x][3];
		}
	}
	return 0;
}

stock IsValidTrafficLightColor(color)
{
	if(color == TLC_GREEN || color == TLC_AMBER || color == TLC_RED)
	{
		return 1;
	}
	return 0;
}

stock IsValidTrafficLightPost(modelid)
{
	for(new x = 0; x < MAX_TRAFFIC_LIGHTS_TYPES; x++)
	{
		if(E_TL_TYPE_DATA[x][0] == modelid)
		{
			return 1;
		}
	}
	return 0;
}

stock LoadTrafficLightsModels()
{
	AddSimpleModel(-1, 19379, TL_TYPE_1, "objects/trafficlights/mtraffic1.dff", "objects/trafficlights/mitraffic.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_2, "objects/trafficlights/cj_traffic_light3.dff", "objects/trafficlights/cj_traffic.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_3, "objects/trafficlights/gay_traffic_light.dff", "objects/trafficlights/gay_xref.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_4, "objects/trafficlights/trafficlight1.dff", "objects/trafficlights/dyntraffic.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_5, "objects/trafficlights/vgsstriptlights1.dff", "objects/trafficlights/vegtrafic2.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_6, "objects/trafficlights/cj_traffic_light5.dff", "objects/trafficlights/cj_traffic.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_7, "objects/trafficlights/cj_traffic_light4.dff", "objects/trafficlights/cj_traffic.txd");
	AddSimpleModel(-1, 19379, TL_TYPE_8, "objects/trafficlights/mtraffic2.dff", "objects/trafficlights/mitraffic.txd");

    AddSimpleModel(-1, 19282, TLC_AMBER, "objects/trafficlights/amberlight.dff", "objects/trafficlights/emptytexture.txd");
}
