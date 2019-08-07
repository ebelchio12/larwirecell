#include "SimChannelSink.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Core/EDProducer.h"

#include "WireCellIface/IDepo.h"
#include "WireCellGen/GaussianDiffusion.h"
#include "WireCellUtil/NamedFactory.h"
#include "WireCellUtil/Units.h"

WIRECELL_FACTORY(wclsSimChannelSink, wcls::SimChannelSink,
		 wcls::IArtEventVisitor, WireCell::IDepoFilter)

using namespace wcls;
using namespace WireCell;

SimChannelSink::SimChannelSink()
  : m_depo(nullptr)
{
  m_mapSC.clear();
  uboone_u = new Pimpos(2400, -3598.5, 3598.5, Point(0,sin(Pi/6),cos(Pi/6)), Point(0,cos(5*Pi/6),sin(5*Pi/6)), Point(94,9.7,5184.98), 1);
  uboone_v = new Pimpos(2400, -3598.5, 3598.5, Point(0,sin(5*Pi/6),cos(5*Pi/6)), Point(0,cos(Pi/6),sin(Pi/6)), Point(94,9.7,5184.98), 1);
  uboone_y = new Pimpos(3456, -5182.5, 5182.5, Point(0,1,0), Point(0,0,1), Point(94,9.7,5184.98), 1);
}

SimChannelSink::~SimChannelSink()
{
  delete uboone_u;
  delete uboone_v;
  delete uboone_y;
}

WireCell::Configuration SimChannelSink::default_configuration() const
{
    Configuration cfg;
    cfg["anode"] = "AnodePlane";
    cfg["rng"] = "Random";
    cfg["start_time"] = -1.6*units::ms;
    cfg["readout_time"] = 4.8*units::ms;
    cfg["tick"] = 0.5*units::us;
    cfg["nsigma"] = 3.0;
    cfg["drift_speed"] = 1.098*units::mm/units::us;
    cfg["uboone_u_to_rp"] = 94*units::mm;
    cfg["uboone_v_to_rp"] = 97*units::mm;
    cfg["uboone_y_to_rp"] = 100*units::mm;
    cfg["u_time_offset"] = 0.0*units::us;
    cfg["v_time_offset"] = 0.0*units::us;
    cfg["y_time_offset"] = 0.0*units::us;
    cfg["use_energy"] = false;
    return cfg;
}

void SimChannelSink::configure(const WireCell::Configuration& cfg)
{
    const std::string anode_tn = cfg["anode"].asString();
    if (anode_tn.empty()) {
        THROW(ValueError() << errmsg{"SimChannelSink requires an anode plane"});
    }
    m_anode = Factory::find_tn<IAnodePlane>(anode_tn);

    const std::string rng_tn = cfg["rng"].asString();
    if (rng_tn.empty()) {
        THROW(ValueError() << errmsg{"SimChannelSink requires a noise source"});
    }
    m_rng = Factory::find_tn<IRandom>(rng_tn);

    m_start_time = get(cfg,"start_time",-1.6*units::ms);
    m_readout_time = get(cfg,"readout_time",4.8*units::ms);
    m_tick = get(cfg,"tick",0.5*units::us);
    m_nsigma = get(cfg,"nsigma",3.0);
    m_drift_speed = get(cfg,"drift_speed",1.098*units::mm/units::us);
    m_uboone_u_to_rp = get(cfg,"uboone_u_to_rp",94*units::mm);
    m_uboone_v_to_rp = get(cfg,"uboone_v_to_rp",97*units::mm);
    m_uboone_y_to_rp = get(cfg,"uboone_y_to_rp",100*units::mm);
    m_u_time_offset = get(cfg,"u_time_offset",0.0*units::us);
    m_v_time_offset = get(cfg,"v_time_offset",0.0*units::us);
    m_y_time_offset = get(cfg,"y_time_offset",0.0*units::us);
    m_use_energy = get(cfg,"use_energy",false);

}

void SimChannelSink::produces(art::EDProducer* prod)
{
    assert(prod);
    prod->produces< std::vector<sim::SimChannel> >("simpleSC");
}

void SimChannelSink::save_as_simchannel(const WireCell::IDepo::pointer& depo){
  Binning tbins(m_readout_time/m_tick, m_start_time, m_start_time+m_readout_time);

  if(!depo) return;

  int ctr = 0;
  while(ctr<1){
    ctr++;
    //      if(ctr % 10000==0){ std::cout<<"COUNTER "<<ctr<<std::endl;}
    for(auto face : m_anode->faces()){
      auto boundbox = face->sensitive();
      if(!boundbox.inside(depo->pos())) continue;

      int plane = -1;
      for(Pimpos* pimpos : {uboone_u, uboone_v, uboone_y}){
        plane++;

	const double center_time = depo->time();
	const double center_pitch = pimpos->distance(depo->pos());

	Gen::GausDesc time_desc(center_time, depo->extent_long() / m_drift_speed);
	{
	  double nmin_sigma = time_desc.distance(tbins.min());
	  double nmax_sigma = time_desc.distance(tbins.max());

	  double eff_nsigma = depo->extent_long() / m_drift_speed>0?m_nsigma:0;
	  if (nmin_sigma > eff_nsigma || nmax_sigma < -eff_nsigma) {
	    break;
	  }
	}

	auto ibins = pimpos->impact_binning();

	Gen::GausDesc pitch_desc(center_pitch, depo->extent_tran());
	{
	  double nmin_sigma = pitch_desc.distance(ibins.min());
	  double nmax_sigma = pitch_desc.distance(ibins.max());

	  double eff_nsigma = depo->extent_tran()>0?m_nsigma:0;
	  if (nmin_sigma > eff_nsigma || nmax_sigma < -eff_nsigma) {
            break;
	  }
	}

	auto gd = std::make_shared<Gen::GaussianDiffusion>(depo, time_desc, pitch_desc);
	gd->set_sampling(tbins, ibins, m_nsigma, 0, 1);

	double xyz[3];
	int id = -10000;
	double energy = 100.0;
	if(depo->prior()){
	  id = depo->prior()->id();
	  if(m_use_energy){ energy = depo->prior()->energy(); }
	}
	else{
	  id = depo->id();
	  if(m_use_energy){ energy = depo->energy(); }
	}

	const auto patch = gd->patch();
	const int poffset_bin = gd->poffset_bin();
	const int toffset_bin = gd->toffset_bin();
	const int np = patch.rows();
	const int nt = patch.cols();

	int min_imp = 0;
	int max_imp = ibins.nbins();

	for (int pbin = 0; pbin != np; pbin++){
	  int abs_pbin = pbin + poffset_bin;
	  if (abs_pbin < min_imp || abs_pbin >= max_imp) continue;

	  int channel = abs_pbin;
	  if(plane == 1){ channel = abs_pbin+2400; }
	  if(plane == 2){ channel = abs_pbin+4800; }

	  auto channelData = m_mapSC.find(channel);
	  sim::SimChannel& sc = (channelData == m_mapSC.end())
	    ? (m_mapSC[channel]=sim::SimChannel(channel))
	    : channelData->second;

	  for (int tbin = 0; tbin!= nt; tbin++){
	    int abs_tbin = tbin + toffset_bin;
	    double charge = patch(pbin, tbin);
	    double tdc = tbins.center(abs_tbin);

	    if(plane == 0){
	      tdc = tdc + (m_uboone_u_to_rp/m_drift_speed) + m_u_time_offset;
	      xyz[0] = depo->pos().x()/units::cm - 94*units::mm/units::cm; //m_uboone_u_to_rp/units::cm;
	    }
	    if(plane == 1){
	      tdc = tdc + (m_uboone_v_to_rp/m_drift_speed) + m_v_time_offset;
	      xyz[0] = depo->pos().x()/units::cm - 97*units::mm/units::cm; //m_uboone_v_to_rp/units::cm;
	    }
	    if(plane == 2){
	      tdc = tdc + (m_uboone_y_to_rp/m_drift_speed) + m_y_time_offset;
	      xyz[0] = depo->pos().x()/units::cm - 100*units::mm/units::cm; //m_uboone_y_to_rp/units::cm;
	    }
	    xyz[1] = depo->pos().y()/units::cm;
	    xyz[2] = depo->pos().z()/units::cm;

	    unsigned int temp_time = (unsigned int) ( (tdc/units::us+4050) / (m_tick/units::us) ); // hacked G4 to TDC
	    charge = abs(charge);
	    if(charge>1){
	      sc.AddIonizationElectrons(id, temp_time, charge, xyz, energy*abs(charge/depo->charge()));
	    }
	  }
	}
      } // plane
    } //face
  }
}

void SimChannelSink::visit(art::Event & event)
{
    std::unique_ptr<std::vector<sim::SimChannel> > out(new std::vector<sim::SimChannel>);

    for(auto& m : m_mapSC){
      out->emplace_back(m.second);
    }

    event.put(std::move(out), "simpleSC");
    m_mapSC.clear();
}

bool SimChannelSink::operator()(const WireCell::IDepo::pointer& indepo,
				WireCell::IDepo::pointer& outdepo)
{
    outdepo = indepo;
    if (indepo) {
        m_depo = indepo;
	save_as_simchannel(m_depo);
    }

    return true;
}
