#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SKIND()
{
	Identifier = "DEFAULT_PT_SKIND";
	Name = "SKIND";
	Colour = PIXPACK(0xE89BBC);
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

	HeatConduct = 5;
	Description = "The Dermis. The middle layer of skin.";

	Properties = TYPE_SOLID|TYPE_BIO;

    DefaultProperties.bio.health = 100;

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
	short r,/* nnx, nny,*/ rx, ry;

    // O2 use by skin itself
    if (RNG::Ref().chance(1, 150)){

		if (parts[i].bio.o2 > 0){
        	parts[i].bio.o2 -= 1;
            parts[i].bio.co2 += 1;
		}
    }

    rx =  RNG::Ref().between(-2, 2);
    ry =  RNG::Ref().between(-2, 2);

    // Resource diffuse
    if (BOUNDS_CHECK && (rx || ry))
    {
        r = pmap[y+ry][x+rx];
        if (r) {
			if (RNG::Ref().chance(1, 2)){
				// Diffuse among bio
				if (sim->elements[TYP(r)].Properties & TYPE_BIO && TYP(r) != PT_BLD){
					short ir = ID(r);

					if (parts[i].bio.o2 > parts[ir].bio.o2){
						parts[i].bio.o2--;
						parts[ir].bio.o2++;
					}
					if (parts[i].bio.co2 > parts[ir].bio.co2){
						parts[i].bio.co2--;
						parts[ir].bio.co2++;
					}
				}
			}
        }
    }

    // Ensure sufficient material for growth
    if (parts[i].bio.o2 > 10){
        // Growth check
        
        if (BOUNDS_CHECK && (rx || ry))
        {
            bool f_skins = 0;

            for (rx=-2; rx<3; rx++){

                if (f_skins) break;

		        for (ry=-2; ry<3; ry++){

                    if (f_skins) break;

                    r = pmap[y+ry][x+rx];
                    if (r){   
                        if (TYP(r) == PT_SKINS){

                            f_skins = true;

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

            if (!f_skins){
                sim->part_change_type(i,x,y,PT_SKINE);
                parts[i].bio.o2 -= 5;
            }
        }  
    }

    // Health management
	if (RNG::Ref().chance(1, 50)){
		// Temp check
		if (parts[i].temp > 323.15){
			int damage = (parts[i].temp - 315) / 5;
			parts[i].bio.health -= damage;
		}
		// Damage check
		if (parts[i].bio.co2 > MAX_CO2 || parts[i].bio.o2 < 1){
			parts[i].bio.health--;
		}
		// Otherwise heal
		else{
            if (parts[i].bio.health < 100){
				parts[i].bio.health++;
			}
		}
	}

	// Death check
	if (parts[i].bio.health < 1){
		sim->part_change_type(i, x, y, PT_DT);
	}

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