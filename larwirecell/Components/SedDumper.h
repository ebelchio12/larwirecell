#ifndef PRIVATE_SEDDUMPER_H
#define PRIVATE_SEDDUMPER_H

#include "lardataobj/Simulation/SimEnergyDeposit.h"
#include <fstream>
#include <string>

// Get SimEnergyDeposit at label from event and dump to filename.
inline void sed_dumper(const art::Event& event,
                       const std::string& label,
                       const std::string& filename,
                       const std::string& prefix="")
{
    if (label.empty() or filename.empty()) {
        return;
    }

    art::Handle<std::vector<sim::SimEnergyDeposit>> sedvh;
    const bool okay = event.getByLabel(label, sedvh);
    if (not okay) {
        return;
    }

    std::fstream fstr(filename, std::fstream::out | std::fstream::app);
    const size_t ndepos = sedvh->size();
    for (size_t index=0; index<ndepos; ++index) {
        auto const& sed = sedvh->at(index);
        fstr << prefix << label
             << " " << event.id()
             << " " << ndepos
             << " " << index
             << " " << event.run() << " " << event.subRun() << " " << event.event()
             << " " << sed.TrackID() << " " << sed.OrigTrackID() << " " << sed.PdgCode()
             << " " << sed.T()
             << " " << sed.X() << " " << sed.Y() << " " << sed.Z()
             << " " << sed.E() << " " << sed.NumElectrons() << " " << sed.NumPhotons()
             << "\n";
    }
}

#endif
