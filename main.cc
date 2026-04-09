#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TString.h>
#include <TObjArray.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>  // For std::system
void Test(std::string word = "Test") {;
    std::cout<<word<<std::endl;
}

void GetBrancheSub(
                    TBranch* branch, 
                    const std::string prefix = "", 
                    std::vector<std::string>* only_sub_branches = nullptr,  // 이렇게 구현하면 안됩니다. 참조라를 기본 초기화할 수는 없습니다.
                    bool print_option = false
                    ) {
    // It's defined as Recursive function. Following is the confition for stopping
    if (!branch) return;
    std::string branch_string = branch-> GetName();
    if (only_sub_branches) {only_sub_branches->push_back(branch_string);}
    // std::cout << prefix << only_sub_branches.size() << std::endl;
    if (print_option == true) {std::cout << prefix << branch_string << std::endl;}
    TObjArray* subBranches = branch->GetListOfBranches();
    for (int i = 0; i < subBranches->GetEntries(); i++) {
        TBranch* subBranch = dynamic_cast<TBranch*>(subBranches->At(i));
        // It's Recursive Funciton, so the Following is needed.
        // It means that Sub-SubBranch will be printed if it exist
        if (subBranch) {
            GetBrancheSub(subBranch, prefix + "", only_sub_branches, print_option);
           }
        }
    }
std::vector<std::string> GetBranches(TTree* tree, bool print_option = false) {
    if (!tree) {
        std::cerr << "Tree is Null..." << std::endl;
    }
    TObjArray* branchList = tree->GetListOfBranches();
    std::vector<std::string> only_sub_branches = {};

    std::cout << "Branches in tree \"" << tree->GetName() << "\":" << std::endl;
    for (int i = 0; i < branchList->GetEntries(); ++i) {
        TBranch* Branch = dynamic_cast<TBranch*>(branchList->At(i));
	    if (Branch) {
            GetBrancheSub(Branch, "", &only_sub_branches, print_option);
            }
        } 
    // 
    return only_sub_branches;
}

void printEvent(TTree* tree, int eventNumber) {
    if (eventNumber < 0 || eventNumber >= tree->GetEntries()) {
        std::cerr << "Invalid event number!" << std::endl;
        return;
    }
    tree->Show(eventNumber);
}

void scanFunc(TTree* tree) {
    tree->Scan();
    }

void HistogramPlot(TTree* tree, const char* branch_name, int event_number,std::string full_folderName = "./") {
    // 트리 유효성 검사
    if (!tree) {
        std::cerr << "Error: TTree is null." << std::endl;
        return;
    }

    // 브랜치 유효성 검사
    if (!tree->GetBranch(branch_name)) {
        std::cerr << "Error: Branch '" << branch_name << "' does not exist in the TTree." << std::endl;
        return;
    }

    // 히스토그램 생성
    std::string hist_name = std::string("hist_") + branch_name; // 히스토그램 이름 생성
    TH1F* hist = new TH1F(hist_name.c_str(), "Histogram", 100, 0, 0); // 100 bins, 자동 범위

    // 트리에서 브랜치 데이터를 히스토그램으로 채우기
    tree->Draw((std::string(branch_name) + ">>" + hist_name).c_str());

    // 스탯 박스 활성화
    gStyle->SetOptStat(1111); // ROOT의 통계 정보 표시

    // 히스토그램을 그리기 위한 캔버스 생성
    TCanvas* canvas = new TCanvas("canvas", "Canvas", 1600, 1200);
    hist->Draw(); // 히스토그램 그리기

    // PDF 파일 이름 생성
    // filename: 입력 파일 이름 (확장자 포함)
    // branch_name: 브랜치 이름
    // event_number: 이벤트 번호
    // size_t dot_pos = filename.find_last_of('.');
    std::string file_basename = "temp";//(dot_pos == std::string::npos) ? filename : filename.substr(0, dot_pos);
    std::ostringstream pdf_name;
    pdf_name << full_folderName << "plot_" << file_basename << "_" << branch_name << "_" << event_number << ".pdf";
    
    std::cout<<pdf_name.str()<<std::endl;            
    // PDF로 저장
    canvas->SaveAs(pdf_name.str().c_str());
    std::cout << "Histogram saved to: " << pdf_name.str() << std::endl;

    // 메모리 해제
    delete hist;
    delete canvas;
}

void Option4Function(TTree* tree) {
    int prompt_option;
	if (!tree){std::cerr << "Tree is Null..." << std::endl;}
    // option : 0 -> one Branch
    // option : 1 -> all Branch
    std::cout<< "Here are selections for you...\n" << std::endl ;
    std::vector<std::string> temp = GetBranches(tree, true);
    while (true) {
        std::string selectedTreeName4Plot;
        std::cout<< "\nEnter the tree name (or '.q' to move Initial Page) ";
        std::cout<< "\nIf you want to generate figure for all branches, Enter \"All\"\n :" << std::endl ;
        std::cin>> selectedTreeName4Plot;
        if (selectedTreeName4Plot == ".q") {
            prompt_option = 0;
            break;
            }
        else if (selectedTreeName4Plot == "All") {
            prompt_option = 0;
            std::vector<std::string> all_branches_name = GetBranches(tree, false);    
            
            // 경고 메시지 출력 (이벤트 번호는 0으로 설정됨)
            std::cout << "Warning: Event number is 0, It's temporary setting." << std::endl;    
            // 저장할 폴더 이름 입력받기
            std::string folder_name;
            std::cout << "Enter the folder name to save histograms: ";
            std::cin >> folder_name;    
            // 디렉토리 생성
            std::string mkdir_command = "mkdir -p " + folder_name;
            int ret_code = std::system(mkdir_command.c_str());
            if (ret_code != 0) {
                std::cerr << "Error creating directory: " << folder_name << std::endl;
                return;}    
            const char* current_branch;
            for (int i = 0; i < all_branches_name.size(); i++) {
                int event_number = 0; // 임시 이벤트 번호 설정
                current_branch = all_branches_name[i].c_str();        
                // 저장 파일 경로 설정
                std::string full_folderName = "./" + folder_name + "/"; // 폴더 내에 저장
                // 히스토그램 생성 및 저장
                HistogramPlot(tree, current_branch, event_number, full_folderName);

                // 출력 메시지
                std::cout << "Histogram for branch \"" << current_branch
                        << "\" saved in folder: " << folder_name << std::endl;
                }    
            }   
        else {
        prompt_option = 1;
        const char* branch_name = selectedTreeName4Plot.c_str();
        const std::string& filename = "temp"; //
        std::cout<<"Warning: Event number is 0, It's temporary setting.";
        int event_number = 0; // temp
        HistogramPlot(tree, branch_name, event_number);
        
        if (prompt_option == true) {
            // imgcat 호출 (PDF 이미지를 출력)
            std::string command = "~/bin/imgcat plot_temp_" + selectedTreeName4Plot + "_" + std::to_string(event_number) +".pdf";
            int ret_code = std::system(command.c_str());
            if (ret_code != 0) {
                std::cerr << "Error executing imgcat. Return code: " << ret_code << std::endl;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: <The binary file>" << " <root_file>" << std::endl;
        return 1;
    }

    // Open ROOT file
    TFile* file = TFile::Open(argv[1]);
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening ROOT file: " << argv[1] << std::endl;
        return 1;
    }
    // Retrieve tree names
    std::vector<std::string> treeNames;
    TList* keys = file->GetListOfKeys();
    for (int i = 0; i < keys->GetEntries(); ++i) {
        TObject* obj = file->Get(keys->At(i)->GetName());
        if (obj->InheritsFrom(TTree::Class())) {
            // tree name을 저장하기 위해서는 위와 같이 조건문을 설정해야한다.
	    treeNames.push_back(obj->GetName());
        }
    }

    // Output tree names
    std::cout << "Available Trees:" << std::endl;
    for (const auto& name : treeNames) {
        std::cout << "  " << name << std::endl;
    }

    // User interaction
    while (true) {
        std::string selectedTreeName;
        std::cout << "\nEnter the tree name (or '.q' to quit): ";
        std::cin >> selectedTreeName;

        if (selectedTreeName == ".q") {
            break;
        }

        // Check if the tree exists
        TTree* tree = dynamic_cast<TTree*>(file->Get(selectedTreeName.c_str()));
        if (!tree) {
            std::cerr << "Tree \"" << selectedTreeName << "\" not found!" << std::endl;
            continue;
        }

        while (true) {
            std::cout << "\nOptions for tree \"" << selectedTreeName << "\":" << std::endl;
            std::cout << "1: Print branches again\n";
            std::cout << "2: Print events\n";
	    	std::cout << "3: Scan root file\n";
			std::cout << "4: ScanMode(It's temporary for class)\n";
            std::cout << "0: Return to tree selection\n";
            std::cout << "Enter your choice: ";
			int option;
            std::cin >> option;

            if (option == 0) {
                break;
            } 
			else if (option == 1) {
                // only for print. So second argument must be true.
                std::vector<std::string> temp = GetBranches(tree, true);
            } 
			else if (option == 2) {
                                                int totalEvents = tree->GetEntries();
                std::cout << "Total events: " << totalEvents << std::endl;
                std::cout << "Enter event number (0-" << totalEvents - 1 << "): ";
                int eventNumber;
                std::cin >> eventNumber;
                printEvent(tree, eventNumber);
	    	} 
			else if (option == 3) {
				tree->Scan();
	    	}
			else if (option == 4 ) {
				Option4Function(tree);
			}
			else {
                std::cerr << "Invalid option!" << std::endl;
            }
        }
    }

    file->Close();
    delete file;
    return 0;
}