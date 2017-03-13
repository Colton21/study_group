//c++ library includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

//ROOT includes - using Root v6.X
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"

//simple load doubles from text file
std::vector < std::vector < double> > load_file(const char * file_name)
{

	std::vector < std::vector < double > > file_vector;
	std::ifstream myfile;
	myfile.open(file_name);

	//some exception handling for file io
	if(!myfile.is_open())
	{
		throw "File is not open!";
	}
	else
	{
		std::cout << "File opened: " << file_name << std::endl;
	}

	int events;
	int x;
	int y;
	int counter = 0;

	std::string line;
	std::vector<double> data_vector;

	//read the data out from the file
	//this readout assumes that values are ',' separated
	//also attempts to handle values which cannot be converted
	//from string to double
	while( !myfile.eof() )
	{
		std::string s;
		if (!getline( myfile, s )) break;
		std::istringstream ss( s );
		while (ss)
		{
			std::string s;
			if (!getline( ss, s, ',' )) break;
			double data;
			try
			{
				data = std::stod(s);
			}
			catch(const std::exception& ex)
			{
				std::cerr << "No convertion to double from string... skipping" << std::endl;
				break;
			}

			//assumes that we have 3 values per line
			if(counter == 3)
			{
				file_vector.push_back(data_vector);
				data_vector.clear();
				counter = 0;
			}
			if(counter != 3)
			{
				data_vector.push_back(data);
				counter++;
			}
		}
	}
	if(!data_vector.empty()) {file_vector.push_back(data_vector); data_vector.clear(); }


	myfile.close();

	return file_vector;

}

//histogram plotting function
void plot_histogram(const char * h_name,
                    const char * h_title,
                    const char * file_name,
                    const char * axis_title_x,
                    const char * axis_title_y,
                    std::vector < std::vector < double> > vector_name
                    )
{
	std::sort(vector_name.begin(), vector_name.end());

	const double max_bin = vector_name.back().at(1);//grab x
	const double min_bin = vector_name.front().at(1);//grab x

	TCanvas * c1 = new TCanvas();
	c1->cd();
	TH1D * h1 = new TH1D(h_name, h_title, 50, min_bin, max_bin);
	TH1D * h2 = new TH1D(h_name, h_title, 50, min_bin, max_bin);

	for(int i = 0; i < vector_name.size(); i++)
	{
		h1->Fill(vector_name.at(i).at(1), vector_name.at(i).at(2));
		h2->Fill(h1->Integral());
	}


	h2->Draw();
	h2->GetXaxis()->SetTitle(axis_title_x);
	h2->GetYaxis()->SetTitle(axis_title_y);
	//h1->Draw("same");
	c1->Print(file_name);

}

//main - call functions here
//try open file -> plot histogram
int main()
{

	const char * in_file = "plot_data.txt";
	std::vector < std::vector < double > > plot_data;

	try
	{
		plot_data = load_file(in_file);
	}
	catch(const char * msg)
	{
		std::cerr << msg << std::endl;
		exit(1);
	}

	plot_histogram("h_test", "h_title", "h_title.pdf", "x", "y", plot_data);

	return 0;
}
