{
	gInterpreter->AddIncludePath("${AMSSRC}/include");
	gSystem->Load("${AMSLIB}/libntuple_slc6_PG_dynamic.so");

    Printf("Current Version    = %s", gROOT->GetVersion());
	Printf("Build Architecture = %s", gSystem->GetBuildArch());
    
    TF1 * f1gs = new TF1("f1gs", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1])");
    f1gs->SetParameters(10., 10.);
    f1gs->SetNpx(100000);

    TF1 * f2gs = new TF1("f2gs", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1]) + ([2]/[3])*TMath::Exp(-0.5*x*x/[3]/[3])");
    f2gs->SetParameters(10., 10., 10., 30.);
    f2gs->SetNpx(100000);

    TF1 * f3gs = new TF1("f3gs", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1]) + ([2]/[3])*TMath::Exp(-0.5*x*x/[3]/[3]) + ([4]/[5])*TMath::Exp(-0.5*x*x/[5]/[5])");
    f3gs->SetParameters(10., 10., 10., 30., 10., 50.);
    f3gs->SetNpx(100000);
    
    TF1 * f4gs = new TF1("f4gs", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1]) + ([2]/[3])*TMath::Exp(-0.5*x*x/[3]/[3]) + ([4]/[5])*TMath::Exp(-0.5*x*x/[5]/[5]) + ([6]/[7])*TMath::Exp(-0.5*x*x/[7]/[7])");
    f4gs->SetParameters(10., 10., 10., 30., 10., 50., 10., 70.);
    f4gs->SetNpx(100000);
    
    TF1 * f5gs = new TF1("f5gs", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1]) + ([2]/[3])*TMath::Exp(-0.5*x*x/[3]/[3]) + ([4]/[5])*TMath::Exp(-0.5*x*x/[5]/[5]) + ([6]/[7])*TMath::Exp(-0.5*x*x/[7]/[7]) + ([8]/[9])*TMath::Exp(-0.5*x*x/[9]/[9])");
    f5gs->SetParameters(10., 10., 10., 30., 10., 50., 10., 70., 10., 100);
    f5gs->SetNpx(100000);
    
    TF1 * f1ags = new TF1("f1ags", "([0]/[1])*(TMath::Exp(-0.5*(x-[2])*(x-[2])/[1]/[1])+TMath::Exp(-0.5*(x+[2])*(x+[2])/[1]/[1]))");
    f1ags->SetParameters(10., 10., 0.);
    f1ags->SetNpx(100000);
    
    //TF1 * f1nags = new TF1("f1nags", "([0]/[1])*TMath::Exp(-0.5*x*x/[1]/[1]) + (0.5*[2]/[3])*(TMath::Exp(-0.5*(x-[4])*(x-[4])/[3]/[3])+TMath::Exp(-0.5*(x+[4])*(x+[4])/[3]/[3]))");
    //f1nags->SetParameters(10., 10., 10., 30., 0.);
    //f1nags->SetNpx(100000);
    
    TF1 * f1nags = new TF1("f1nags", "([0]/[3])*([1]*TMath::Exp(-0.5*x*x/[3]/[3]) + TMath::Sqrt(2)*(TMath::Exp(-(x-[2])*(x-[2])/[3]/[3]) + TMath::Exp(-(x+[2])*(x+[2])/[3]/[3])))");
    f1nags->SetParameters(10., 1., 0., 10.);
    f1nags->SetNpx(100000);
    
    TF1 * feloss = new TF1("feloss", "[0] * TMath::Power( ([2]/x)/[1]/[1], ([2]/x)/[1]/[1] ) / TMath::Gamma( ([2]/x)/[1]/[1] ) * TMath::Exp(-(([2]/x)/[1]/[1]) * ((x-[2])/[3] + TMath::Exp(-(x-[2])/[3])) )");
    feloss->SetParameters(1000., 1.0, 0.0015, 0.0002); 
    
    TF1 * fseloss = new TF1("fseloss", "[0] * TMath::Power( (2*[2]/(x+[2]))/[4]/[4], (2*[2]/(x+[2]))/[4]/[4] ) / TMath::Gamma( (2*[2]/(x+[2]))/[4]/[4] ) * TMath::Exp(-( (([2]/x)/[1]/[1]) * ((x-[2])/[3] + TMath::Exp(-(x-[2])/[3])) ))");
    fseloss->SetParameters(1000., 1.0, 0.15, 0.02, 1.0);
    fseloss->FixParameter(4, 1.0);
   
    TF1 * fvavilov = new TF1("fvavilov", "[0] * TMath::Vavilov((x-[1])/[2], [3], [4])");
    fvavilov->SetParameters(1000, 0.15, 0.02, 1.0, 1.0);

    //const char* tt = "(1.0/[1]/[1]) * (([2]/[3]) / ((abs((x-[2])/[3])) + ([2]/[3])) )^[4]";
    //TF1 * feloss2 = new TF1("feloss2", Form("[0] * TMath::Power(%s, %s) / TMath::Gamma(%s) * TMath::Exp(-(%s) * ((x-[2])/[3] + TMath::Exp(-(x-[2])/[3])) )", tt, tt, tt, tt));
    //feloss2->SetParameters(1000., 1.0, 0.0015, 0.0002, 1.0); 
    
    
    
    TF1* fel = new TF1("fel", "[0] * (1+x*x)^[2] * ([1] - (1+x*x)^(-[2]) - TMath::Log([3] + abs(x)^[4]))");
    fel->SetParameters(10, 6.5, 1.0, 10.0, 1.0);
    
    if (std::atof(gROOT->GetVersion()) < 6.00) return;

	//---------------//
	//  User Define  //
	//---------------//
	gROOT->LoadMacro("$AMSProjLibs/CPPLibs/CPPLibs.h");
	gROOT->LoadMacro("$AMSProjLibs/ROOTLibs/ROOTLibs.h");
	gROOT->LoadMacro("$AMSProjLibs/TRACKLibs/TRACKLibs.h");
	gROOT->ProcessLine("MGROOT::LoadDefaultEnvironment();");
	gROOT->ProcessLine("using namespace MGROOT;");
	gROOT->ProcessLine("using namespace TrackSys;");
}
