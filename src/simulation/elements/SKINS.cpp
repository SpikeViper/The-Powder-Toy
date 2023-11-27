#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SKINS()
{
	Identifier = "DEFAULT_PT_SKINS";
	Name = "SKNS";
	Colour = PIXPACK(0xE0E69A);
	MenuVisible = 1;
	MenuSection = SC_BIO;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 2;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 3;
	Description = "The Subcutaneous layer. The bottom layer of skin.";

    // Bio stuff

	Max_O2 = 100;
	Max_CO2 = 100;
	
	Max_Health = 100;

	DefaultProperties.bio.o2 = Max_O2;
	DefaultProperties.bio.co2 = 0;
	DefaultProperties.bio.health = Max_Health;

	Properties = TYPE_SOLID|TYPE_BIO;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 7.0f;
	HighPressureTransition = PT_NONE;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
    Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{

    // O2 use itself
    Biology::UseO2(150, UPDATE_FUNC_IN);
    // Diffuse resources
	Biology::DiffuseResources(2, 2, UPDATE_FUNC_IN);

	int rx, ry, r;

	rx =  RNG::Ref().between(-2, 2);
    ry =  RNG::Ref().between(-2, 2);

    // Ensure sufficient material for growth
    if (parts[i].bio.o2 > 10){
        // Growth check
        
        if (BOUNDS_CHECK && (rx || ry))
        {
            bool f_meat = 0;

            for (rx=-2; rx<3; rx++){

                if (f_meat) break;

		        for (ry=-2; ry<3; ry++){

                    if (f_meat) break;

                    r = pmap[y+ry][x+rx];
                    if (r){   
                        if (TYP(r) == PT_MEAT){

                            f_meat = true;

                            // Try to grow in one random surrounding spot
                            int rrx = RNG::Ref().between(-2, 2);
                            int rry = RNG::Ref().between(-2, 2);

                            int rr = pmap[y+rry][x+rrx];
                            if (!rr){
                                sim->create_part(-1,x+rrx,y+rry,PT_SKIND);
                                parts[i].bio.o2 -= 5;
                            }
                        }
                    }
                }
            }

            if (!f_meat){
                sim->part_change_type(i,x,y,PT_SKIND);
                parts[i].bio.o2 -= 5;
            }
        }  
    }

	// Radiation damage
	Biology::DoRadiationDamage(2, 2, UPDATE_FUNC_IN);
    // Damage from extreme heat or cold
	// Skin is a bit more resilient against temperature
	Biology::DoHeatDamage(12, 323.15, 273, UPDATE_FUNC_IN);
	// Damage from lack of O2 or too much CO2
	Biology::DoRespirationDamage(100, UPDATE_FUNC_IN);
	// Heal naturally
	Biology::DoHealing(100, UPDATE_FUNC_IN);
	// Death check
	Biology::HandleDeath(UPDATE_FUNC_IN);

	return 0;
}


static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Life mix
	*colr = int(*colr * (cpart->bio.health) / 100.0f);
	*colg = int(*colg * (cpart->bio.health) / 100.0f);
	*colb = int(*colb * (cpart->bio.health) / 100.0f);

	return 0;
}
