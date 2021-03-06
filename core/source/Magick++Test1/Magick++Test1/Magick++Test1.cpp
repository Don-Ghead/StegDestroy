// Magick++Test1.cpp : Defines the entry point for the console application.
//
// Simple test project that can be used to verify the Magick++ component of the solution 
// is functioning as intended. 

#include "stdafx.h"
#include <Magick++.h>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace Magick;
using namespace std;

#define QUALITY_INCREMENT 1
#define MIN_JPG_QUALITY 0
#define MAX_JPG_QUALITY 100

int main(int argc, char** argv)
{
	Magick::InitializeMagick(*argv);
	string image_path("..\\resources\\test_images\\");
	string image_path_out("..\\resources\\img_out\\");
	
		try 
		{
			// Construct an image instance first so that we don't have to worry
			// about object construction failure due to a minor warning exception
			// being thrown.
			Magick::Image jpg_image_master, bmp_image_master, gif_image_master;
			int num_img_formats;
			
			try 
			{
				// Try reading image file
				jpg_image_master.read(image_path + "Cover_embed.jpg");
				bmp_image_master.read(image_path + "Cover_embed.bmp");
				gif_image_master.read(image_path + "Cover_embed.gif");
				num_img_formats = 3; //This is used later on for chrono processing
				cout << "Successfully read images, writing to BLOB now" << endl;
			}
			catch (Magick::WarningCoder &warning)
			{
				// Process coder warning while loading file (e.g. TIFF warning)
				// Maybe the user will be interested in these warnings (or not).
				// If a warning is produced while loading an image, the image
				// can normally still be used (but not if the warning was about
				// something important!)
				cerr << "Coder Warning: " << warning.what() << endl;
			}
			catch (Magick::Warning &warning)
			{
				// Handle any other Magick++ warning.
				cerr << "Warning: " << warning.what() << endl;
			}
			catch (Magick::ErrorFileOpen &error)
			{
				// Process Magick++ file open error
				cerr << "Error: " << error.what() << endl;
			}

			try 
			{
				std::chrono::duration<double> jpg_total, gif_total, bmp_total;
				int num_img_iter = 0;
				auto start = std::chrono::system_clock::now();
				for (int i = MAX_JPG_QUALITY; i >= MIN_JPG_QUALITY; i -= QUALITY_INCREMENT)
				{
					auto jpg_start = std::chrono::system_clock::now();
					//start with the simplest, just writing out the jpeg in seperate quality 
					Magick::Image jpg_copy(jpg_image_master);
					jpg_copy.quality(i);	
					jpg_copy.write(image_path_out + "JPG" + to_string(i) + "toJPG.jpg");
					auto jpg_end = std::chrono::system_clock::now();

					//the next two follow the same process
					//Create copy of master image, change quality & encode using jpeg to BLOB
					//Read blob back into alternate image & return to original format to write out 
					auto gif_start = std::chrono::system_clock::now();
					Magick::Image gif_copy(gif_image_master), gif_image_altered;
					Magick::Blob gif_blob;
					gif_copy.quality(i);
					gif_copy.magick("JPEG");
					gif_copy.write(&gif_blob);
					gif_image_altered.read(gif_blob);
					gif_image_altered.magick("GIF");
					gif_image_altered.write(image_path_out + "JPG" + to_string(i) + "BLOBtoGIF.gif");
					auto gif_end = std::chrono::system_clock::now();

					auto bmp_start = std::chrono::system_clock::now();
					Magick::Image bmp_copy(bmp_image_master), bmp_image_altered;
					Magick::Blob bmp_blob;
					bmp_copy.quality(i);
					bmp_copy.magick("JPEG");
					bmp_copy.write(&bmp_blob);
					bmp_image_altered.read(bmp_blob);
					bmp_image_altered.magick("BMP");
					bmp_image_altered.write(image_path_out + "JPG" + to_string(i) + "BLOBtoBMP.bmp");
					auto bmp_end = std::chrono::system_clock::now();

					++num_img_iter;
					jpg_total += jpg_end - jpg_start;
					gif_total += gif_end - gif_start;
					bmp_total += bmp_end - bmp_start;
				}
				auto end = std::chrono::system_clock::now();

				cout << "Number of iterations: " << num_img_iter << endl << endl;

				//Compute total time for all images to be processed
				std::chrono::duration<double> elapsed_seconds = end - start;
				cout << endl << "Total Elapsed time for processing all images: " << elapsed_seconds.count() << "sec" << endl;
				auto calc_total = jpg_total + gif_total + bmp_total;
				cout << "Calculated total based on each format chrono combined: " << calc_total.count() << endl;

				cout << "General overhead = " << (elapsed_seconds - calc_total).count() << endl;
				cout << "_______________________________________________" << endl << endl;

				cout << endl << "Total elapsed time for image formats;" << endl << endl;
				cout << "\tJPG Total: " << jpg_total.count() << endl;
				cout << "\tGIF Total: " << gif_total.count() << endl;
				cout << "\tBMP Total: " << bmp_total.count() << endl << endl;

				//Compute avg time for each img type to be processed
				auto jpg_avg = jpg_total / num_img_iter;
				auto gif_avg = gif_total / num_img_iter;
				auto bmp_avg = bmp_total / num_img_iter;				

				cout << endl << "Average elapsed time for image formats;" << endl << endl;
				cout << "\tJPG Average: " << jpg_avg.count() << endl;
				cout << "\tGIF Average: " << gif_avg.count() << endl;
				cout << "\tBMP Average: " << bmp_avg.count() << endl << endl;
			}
			catch (Magick::Exception & error)
			{
				// Handle problem while writing outfile.
				cerr << "Caught Magick++ exception: " << error.what() << endl;
			}
		}
		catch (std::exception & error)
		{
			// Process any other exceptions derived from standard C++ exception
			cerr << "Caught C++ STD exception: " << error.what() << endl;
		}
		catch (...)
		{
			// Process *any* exception (last-ditch effort). There is not a lot
			// you can do here other to retry the operation that failed, or exit
		}


	cout << "Verifying Magick++ is working as intended..." << endl;

	
}

