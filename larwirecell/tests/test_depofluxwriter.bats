#!/usr/bin/env bats

setup_file () {
    mydir="$(dirname "$BATS_TEST_FILENAME")"

    export FHICL_FILE_PATH="$mydir/depofluxwriter/fcl:$FHICL_FILE_PATH"
    export WIRECELL_PATH="$mydir/depofluxwriter/cfg:$WIRECELL_PATH"

    echo $FHICL_FILE_PATH|tr : '\n'
    ls -l
    pwd

    if [[ -f single_gen_dunefd.root ]] ; then
        echo "Skipping prodsingle_sim_dunefd_depoflux.fcl" 1>&3
    else
        art -n 1 -c $mydir/depofluxwriter/fcl/prodsingle_sim_dunefd_depoflux.fcl 1>&3
    fi
}

@test "Assure SimEnergyDeposits match" {
    local src=wcls-sim-drift-deposource.log
    local tgt=wcls-sim-drift-depowriter.log
    diff -u \
     <(grep '^SimDepoSetSource IonAndScint sed' $src | sed 's,SimDepoSetSource IonAndScint,,') \
     <(grep '^DepoFluxWriter IonAndScint sed'   $tgt | sed 's,DepoFluxWriter IonAndScint,,')

}
@test "Assure IDepos match" {
    local src=wcls-sim-drift-deposource.log
    local tgt=wcls-sim-drift-depowriter.log
    diff -u \
     <(grep '^SimDepoSetSource IonAndScint depo 0' $src| sed 's,SimDepoSetSource IonAndScint depo 0,,' ) \
     <(grep '^DepoFluxWriter simpleSC depo 1'   $tgt | sed 's,DepoFluxWriter simpleSC depo 1,,')

}
