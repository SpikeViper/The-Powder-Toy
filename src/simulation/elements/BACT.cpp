#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_BACT()
{
	Identifier = "DEFAULT_PT_BACT";
	Name = "BACT";
	Colour = PIXPACK(0xF433FF);
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
	DefaultProperties.bio.health = 500;
	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 29;
	Description = "Bacteria. Infects living things (genes stored in tmp) (WIP).";

	Properties = TYPE_LIQUID|PROP_NEUTPENETRATE|TYPE_BIO|TYPE_DISEASE;

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
	short r, rx, ry;

    rx = RNG::Ref().between(-2, 2);
    ry = RNG::Ref().between(-2, 2);

    // O2 use itself (made very slow for somewhat accuracy)
    if (RNG::Ref().chance(1, 1000)){

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
			// Steal O2 and offload CO2
			if (t != PT_BACT && sim->elements[t].Properties & TYPE_BIO){
				// Take oxygen
				if (parts[ir].bio.o2 > 0 && parts[i].bio.o2 < (MAX_O2 * 4)){
					parts[ir].bio.o2--;
					parts[i].bio.o2++;
				}
				// Give co2
				if (parts[i].bio.co2 > 0){
					parts[i].bio.co2--;
					parts[ir].bio.co2++;
				}

				// Kill other cells for resources
				parts[ir].bio.health -= 5;
				parts[i].bio.o2 += 2;
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
		if (parts[i].bio.co2 > MAX_CO2 * 4 || parts[i].bio.o2 < 1){
			parts[i].bio.health--;
		}
		// Otherwise heal
		else{
			if (parts[i].bio.health < 500 && parts[i].bio.o2 > 1){
				parts[i].bio.health++;
				parts[i].bio.o2--;
			}
		}
	}

	// Death check
	if (parts[i].bio.health < 1){
		sim->part_change_type(i, x, y, PT_DT);
	}

	// Multiply check
	if (RNG::Ref().chance(1, 200)){

		rx =  RNG::Ref().between(-3, 3);
    	ry =  RNG::Ref().between(-3, 3);

		if (parts[i].bio.o2 > (MAX_O2 * 3) 
			&& BOUNDS_CHECK && (rx || ry))
    	{
        	r = pmap[y+ry][x+rx];
			short t = TYP(r);
			short ir = ID(r);

			// Empty
			if (t == 0){
				sim->create_part(ir, x, y, PT_BACT);
				parts[ir].bio.o2 = MAX_O2;
				parts[i].bio.o2 = 20;
				parts[ir].bio.health = 250;
			}
			else if (t == PT_DT || t == PT_BLD){
				sim->part_change_type(ir, x, y, PT_BACT);
				parts[ir].bio.o2 = MAX_O2;
				parts[i].bio.o2 = 20;
				parts[ir].bio.health = 250;
			}
		}
	}


	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
    // Oxygen
    short o = cpart->bio.o2;

    // C02
    short c = cpart->bio.co2;

	//short q = cpart->bio.o2;

	short d = o - c;


	*colr = (int)fmin(d, 20) * 2;
	*colg = (int)fmin(d, 20) * 20;
	*colb = (int)fmin(d, 20) * 12;
	*pixel_mode |= PMODE_BLUR;

	//*colr = int(*colr * (cpart->bio.health) / 100.0f);
	//*colg = int(*colg * (cpart->bio.health) / 100.0f);
	//*colb = int(*colb * (cpart->bio.health) / 100.0f);

	return 0;
}

/*
int evaluateGenes(int gene, int genome)
{
	if (gene == 1)
	{
		geneValue = genome % 256;
		return geneValue;
	}
}

int modifyGenes(int gene, int newValue, int genome)
{
	
}
*/


