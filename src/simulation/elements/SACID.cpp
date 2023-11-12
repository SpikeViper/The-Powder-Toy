#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SACID()
{
	Identifier = "DEFAULT_PT_SACID";
	Name = "SACD";
	Colour = PIXPACK(0x33FF66);
	MenuVisible = 1;
	MenuSection = SC_BIO;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.20f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 40;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	PhotonReflectWavelengths = 0x1FE001FE;

	Weight = 10;

	HeatConduct = 34;
	Description = "Stomach acid. Dissolves food and bio.";

	Properties = TYPE_LIQUID|PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
    int rx =  RNG::Ref().between(-1, 1);
	int ry =  RNG::Ref().between(-1, 1);

    if (BOUNDS_CHECK && (rx || ry))
    {
        int r = pmap[y+ry][x+rx];
        if (!r)
            return 0;

        int type = TYP(r);
        Element re = sim->elements[type];

        if (type == PT_STOM || type == PT_SACID || type == PT_MUCO || type == PT_SVLV)
        {
            return 0;
        }

        if ((re.Properties & TYPE_BIO) || (re.Properties & TYPE_FOOD))
        {
            sim->kill_part(i);
            sim->part_change_type(ID(r),x+rx,y+ry,PT_DIGE);
        }
    }

	return 0;
}
