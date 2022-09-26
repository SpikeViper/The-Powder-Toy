#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_WBLD()
{
	Identifier = "DEFAULT_PT_WBLD";
	Name = "WBLD";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_BIO;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.10f;
	HotAir = 0.000f  * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 10;
	DefaultProperties.bio.health = 200;
	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 29;
	Description = "White blood cells. Fight infection, but use a lot of resources. Multiply if infection is detected.";

	Properties = TYPE_LIQUID|PROP_NEUTPENETRATE|TYPE_BIO;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry;

    rx =  RNG::Ref().between(-4, 4);
    ry =  RNG::Ref().between(-4, 4);

    // O2 use by white blood itself
    if (RNG::Ref().chance(1, 100)){

		if (parts[i].bio.o2 > 0){
        	parts[i].bio.o2 -= 1;
			parts[i].bio.co2 += 1;
		}
    }

    
    if (BOUNDS_CHECK && (rx || ry))
    {
        r = pmap[y+ry][x+rx];

		short t = TYP(r);
		short ir = ID(r);

        if (r){
			// Take resources from blood
			if (t  == PT_BLD){
				// Take oxygen
				if (parts[ir].bio.o2 > 0 && parts[i].bio.o2 < MAX_O2 * 2){
					parts[i].bio.o2++;
					parts[ir].bio.o2--;
				}
				// Give co2
				if (parts[i].bio.co2 > 0){
					parts[i].bio.co2--;
					parts[ir].bio.co2++;
				}
				
			}
            // Kill foreign biological objects
            else if (sim->elements[t].Properties & TYPE_DISEASE){
                // Damage disease
				parts[ir].bio.health -= 75;

                // Multiply (if possible)
                if (RNG::Ref().chance(1, 100)){
                    rx =  RNG::Ref().between(-2, 2);
                    ry =  RNG::Ref().between(-2, 2);

                    int t = TYP(r);
                    int ir = ID(r);

                    if (parts[i].bio.o2 > MAX_O2){
                        // Can replace nothing or blood
                        if (t == 0){
                            sim->create_part(ir, x, y, PT_WBLD);
                            parts[ir].bio.o2 = MAX_O2 / 2;
                            parts[i].bio.o2 = MAX_O2 / 2;
                            parts[ir].bio.health = 200;
                        }
                        else if (t == PT_BLD){
                            sim->part_change_type(ir, x, y, PT_WBLD);
                            parts[ir].bio.o2 = MAX_O2 / 2;
                            parts[i].bio.o2 = MAX_O2 / 2;
                            parts[ir].bio.health = 200;
                        }
                    }
                }
		    }
		}
    }

	// Health management
	if (RNG::Ref().chance(1, 100)){

		// Temp check
		if (parts[i].temp > 323.15){
			int damage = (parts[i].temp - 315) / 5;
			parts[i].bio.health -= damage;
		}
		// Damage check
		if (parts[i].bio.co2 > MAX_CO2 || parts[i].bio.o2 < 1){
			parts[i].bio.health--;
		}
		// Otherwise heal (Why make it not use O2 to heal?)
		else{
			if (parts[i].bio.health < 500){
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
    // Oxygen
    short l = cpart->bio.health;

	*colr = (l / 200) * 255;
	*colg = (l / 200) * 255;
	*colb = (l / 200) * 255;
	*pixel_mode |= PMODE_BLUR;

	//*colr = int(*colr * (cpart->bio.health) / 100.0f);
	//*colg = int(*colg * (cpart->bio.health) / 100.0f);
	//*colb = int(*colb * (cpart->bio.health) / 100.0f);

	return 0;
}
