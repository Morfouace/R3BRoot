void testNeulandSimulation(const UInt_t particleID = 2112,
                           const UInt_t nParticles = 4,
                           const Double_t momentum = 0.7,
                           const UInt_t nEvents = 10,
                           const TString basePath = ".",
                           const TString baseName = "test")
{
    TStopwatch timer;
    timer.Start();

    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Output files
    const TString outFile = basePath + "/" + baseName + ".sim.root";
    const TString parFile = basePath + "/" + baseName + ".par.root";

    // Basic simulation setup
    auto run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetOutputFile(outFile);
    run->SetMaterials("media_r3b.geo");

    // Geometry: Cave
    auto cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // Geometry: Neuland
    run->AddModule(new R3BNeuland("neuland_demo_11m.geo.root"));

    // Primary particle generator
    auto boxGen = new FairBoxGenerator(particleID, nParticles);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetPRange(momentum, momentum);
    auto primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // Further setup options and initialization
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
    run->SetStoreTraj(kTRUE);
    run->Init();

    // Connect runtime parameter file
    auto parFileIO = new FairParRootFileIo(kTRUE);
    parFileIO->open(parFile);
    auto rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();

    // Simulate
    run->Run(nEvents);

    // Report
    timer.Stop();
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << endl << endl;
}
