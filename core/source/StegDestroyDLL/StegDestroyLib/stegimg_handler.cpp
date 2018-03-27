#include "stegimg_handler.hpp"
#include "steg_data_types.hpp"

using namespace srl;

srl::Srl_jpgscrub_stegimg_handler::Srl_jpgscrub_stegimg_handler(std::vector<Srl_steg_image>& img_data_v)
{
}

srl::Srl_jpgscrub_stegimg_handler::~Srl_jpgscrub_stegimg_handler()
{
}

void srl::Srl_jpgscrub_stegimg_handler::mat_to_magick(cv::Mat & cv_mat, Magick::Image magick_img)
{
}

void srl::Srl_jpgscrub_stegimg_handler::magick_to_mat(Magick::Image & magick_img, cv::Mat & cv_mat)
{
}

Srl_exception_status srl::Srl_jpgscrub_stegimg_handler::encode_to_format(Srl_img_format_enum img_format)
{
	return Srl_exception_status();
}
