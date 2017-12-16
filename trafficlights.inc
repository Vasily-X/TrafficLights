/*
    TrafficLights v0.01 wrote by RIDE2DAY
    Official thread: http://forum.sa-mp.com/showthread.php?t=642022

    Many thanks to:
    	- Southclaws
    	- |stuntman|
    	- mschnitzer
*/

#if defined _trafficlights_included
	#endinput
#endif
#define _trafficlights_included


#include <YSI-Includes\YSI\y_hooks>
#include <YSI-Includes\YSI\y_iterate>


// ================================== [DEFINITIONS] ================================== //
#define INVALID_TL_ID				-1

#define MAX_TRAFFIC_LIGHTS_TYPES	2    	
#define MAX_TRAFFIC_LIGHTS			50


#if !defined TLC_GREEN
	#define TLC_GREEN					19283
#endif

#if !defined TLC_AMBER
	#define TLC_AMBER					-2056
#endif

#if !defined TLC_RED
	#define TLC_RED						19282
#endif


#define TL_TYPE_1						-2055
#define TL_TYPE_2						-2056


// ================================== [VARIABLES] ================================== //
enum E_TL_DATA
{
	tl_model_id,
	tl_object_id,
	tl_color,
	tl_light_object[2]
}
static TL_Data[MAX_TRAFFIC_LIGHTS][E_TL_DATA];
static Iterator:TL_Data<MAX_TRAFFIC_LIGHTS>;

static E_TL_TYPE_AMOUNT[MAX_TRAFFIC_LIGHTS_TYPES][2] =
{
	{TL_TYPE_1, 2},
	{TL_TYPE_2, 1}
};

static Float:E_TL_TYPE_1_OFFSETS[6][3] =
{
	{-0.630000, -0.299999, 2.649999}, // green  0
	{-1.020000, -0.299999, 2.649999}, // amber  1
 	{-1.410000, -0.299999, 2.649999}, // red 	2
	{-7.300002, 0.310000, 2.649999}, // green 	3    
	{-6.909999, 0.310000, 2.649999}, // amber 	4
	{-6.509998, 0.310000, 2.649999} // red 		5
};

// ================================== [NEW FUNCTIONS] ================================== //
stock CreateTrafficLight(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, default_color = TLC_GREEN)
{
	if(!IsValidTrafficLightPost(modelid))
	{
		print("** [TrafficLights] The traffic light couldn't be created, the post model isn't correct.");
		return INVALID_TL_ID;
	}

	if(!IsValidTrafficLightColor(default_color))
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

	TL_Data[idx][tl_model_id] = modelid;
	TL_Data[idx][tl_color] = default_color;

	#if defined _streamer_included
		TL_Data[idx][tl_object_id] = CreateDynamicObject(modelid, x, y, z, rx, ry, rz);
	#else

		TL_Data[idx][tl_object_id] = CreateObject(modelid, x, y, z, rx, ry, rz);
	#endif

	SetTrafficLightColor(idx, default_color);

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

	new arr_modifier = 0;
	new lights_amount = GetPostRequiredLights(TL_Data[idx][tl_model_id]);

	new Float:pos_off[3];

	switch(color)
	{
		case TLC_GREEN: arr_modifier = 0;
		case TLC_AMBER: arr_modifier = 1;
		case TLC_RED: arr_modifier = 2;
	}

	DisableTrafficLight(idx);

	for(new x = 0; x < lights_amount; x++)
	{
		switch(TL_Data[idx][tl_model_id])
		{
			case TL_TYPE_1:
			{
				pos_off[0] = E_TL_TYPE_1_OFFSETS[arr_modifier][0];
				pos_off[1] = E_TL_TYPE_1_OFFSETS[arr_modifier][1];
				pos_off[2] = E_TL_TYPE_1_OFFSETS[arr_modifier][2];
			}
		}

		#if defined _streamer_included

			TL_Data[idx][tl_light_object][x] = CreateDynamicObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachDynamicObjectToObject(TL_Data[idx][tl_light_object][x], TL_Data[idx][tl_object_id], pos_off[0], pos_off[1], pos_off[2], 0.0, 0.0, 0.0, 0);

		#else

			TL_Data[idx][tl_light_object][x] = CreateObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			AttachObjectToObject(TL_Data[idx][tl_light_object][x], TL_Data[idx][tl_object_id], pos_off[0], pos_off[1], pos_off[2], 0.0, 0.0, 0.0, 0);

		#endif

		arr_modifier += 3;
	}
	return 1;
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
		#if defined _streamer_included

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

stock GetPostRequiredLights(tl_type)
{
	for(new x = 0; x < MAX_TRAFFIC_LIGHTS_TYPES; x++)
	{
		if(E_TL_TYPE_AMOUNT[x][0] == tl_type)
		{
			return E_TL_TYPE_AMOUNT[x][1];
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
		if(E_TL_TYPE_AMOUNT[x][0] == modelid)
		{
			return 1;
		}
	}
	return 0;
}