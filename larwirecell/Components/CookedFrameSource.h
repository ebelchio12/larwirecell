/** A WCT component which is a source of "cooked" frames which it
 * produces by also being an art::Event visitor.
 *
 * Cooked means that the waveforms are taken from the art::Event as a
 * labeled std::vector<recob::Wire> collection.
 */

#ifndef LARWIRECELL_COMPONENTS_COOKEDFRAMESOURCE
#define LARWIRECELL_COMPONENTS_COOKEDFRAMESOURCE

#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/IFrameSource.h"
#include "WireCellUtil/Logging.h"
#include "larwirecell/Interfaces/IArtEventVisitor.h"

#include "canvas/Utilities/InputTag.h"

#include <deque>
#include <string>
#include <vector>

namespace wcls {
  class CookedFrameSource : public IArtEventVisitor,
                            public WireCell::IFrameSource,
                            public WireCell::IConfigurable {
  public:
    CookedFrameSource();
    virtual ~CookedFrameSource();

    /// IArtEventVisitor
    virtual void visit(art::Event& event);

    /// IFrameSource
    virtual bool operator()(WireCell::IFrame::pointer& frame);

    /// IConfigurable
    virtual WireCell::Configuration default_configuration() const;
    virtual void configure(const WireCell::Configuration& config);

  private:
    std::deque<WireCell::IFrame::pointer> m_frames;
    art::InputTag m_inputTag;
    double m_tick;
    int m_nticks;
    std::vector<std::string> m_frame_tags;
    art::InputTag m_wiener_inputTag;
    art::InputTag m_gauss_inputTag;
    std::vector<art::InputTag> m_badmasks_inputTag;
    art::InputTag m_threshold_inputTag;
    bool m_cmm_setup{false}; // true means use cmm setup
    std::string m_wiener_tag{"wiener"};
    std::string m_gauss_tag{"gauss"};
    std::string m_cmm_tag; // for now use a single tag for output cmm

    WireCell::Log::logptr_t l;
  };

}

#endif
