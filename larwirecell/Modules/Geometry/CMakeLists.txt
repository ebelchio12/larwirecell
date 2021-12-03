#add_subdirectory(tools)

art_make( MODULE_LIBRARIES larcorealg_Geometry
                           larcore_Geometry_Geometry_service
                           lardataobj_Simulation
                           lardata_Utilities
                           larevt_Filters
                           lardataobj_RawData
                           larevt_CalibrationDBI_Providers
                           art::Framework_Core
                           art::Framework_Principal
                           art::Framework_Services_Registry
                           art::Framework_Services_Optional_RandomNumberGenerator_service
                           art::Persistency_Common
                           art::Persistency_Provenance
                           art::Utilities
                           canvas::canvas
                           messagefacility::MF_MessageLogger
                           fhiclcpp::fhiclcpp
                           cetlib::cetlib
                           cetlib_except::cetlib_except
                           ROOT::Geom
                           ROOT::XMLIO
                           ROOT::Gdml
        )

#install_headers()
# install_fhicl()
install_source()
