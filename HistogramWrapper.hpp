#ifndef HISTOGRAMWRAPPER_HPP
#define HISTOGRAMWRAPPER_HPP


#include "numerical_string_format.hpp"
#include "function_debug.hpp"


#include <memory>
#include <map>


#include <TH1.h>
//#include <TFile.h>
#include <TCanvas.h>


// TODO: create TH1 data class
class TH1Data
{

    // in old version had: TH1*, weight, line width, line width override, color flag, color flag enable


    public:

    TH1*& Histogram()
    {
        return _histogram_;
    }


    private:

    TH1* _histogram_;
    //Int_t _line_width_;
    //Int_t _color_;

};



// particularly designed for canvas output
// multiple histograms
// constructor:
// 
class HistogramWrapper2
{
    
    public:

    HistogramWrapper2(const std::string& name,
                      const std::string& x_axis_label_text,
                      const std::string& y_axis_label_text,
                      const std::string& draw_opt)
        : _name_{name}
        , _x_axis_label_text_{x_axis_label_text}
        , _y_axis_label_text_{y_axis_label_text}
        , _draw_opt_{draw_opt}
        , _log_mode_{false}
        , _y_axis_max_override_{false}
    {
        // set the axis label font size to default
        _axis_label_font_size_ = _DEFAULT_AXIS_LABEL_FONT_SIZE_;

        // set the axis label font type to default
        _axis_label_font_ = _DEFUALT_AXIS_LABEL_FONT_;

        // set the axis number font size to default
        _axis_number_font_size_ = _DEFAULT_AXIS_NUMBER_FONT_SIZE_;

        // set the canvas x and y size
        _canvas_size_x_ = _DEFAULT_CANVAS_SIZE_X_;
        _canvas_size_y_ = _DEFAULT_CANVAS_SIZE_Y_;

    }

    ~HistogramWrapper2()
    {
        Iterator_t it{_histogram_.begin()};
        for(; it != _histogram_.end(); ++ it)
        {
            delete it->_histogram_;
            it->_histogram_ = nullptr;
        }

        _histogram_.clear();
    }


    // setting the font size is considered a seperate operation from the constructor
    void SetFontSize(/*argument*/)
    {

    }

    // over-ride y axis limit (maximum)
    // TODO: implement
    void SetYaxisMax(const Double_t value)
    {
        _y_axis_max_override_ = true;
        _y_axis_max_ = value;
    }

    void SetLogMode(const bool log_mode = true)
    {
        _log_mode_ = log_mode;
    }

    // use name passed to constructor for filename
    void Canvas(const std::string& directory = "")
    {
        Canvas(_name_);   
    }

    // specify custom filename (used for producing multiple canvas output
    // from one instance of a class - eg; log mode and linear mode canvas
    // output)
    void Canvas(const std::string& filename, const std::string& directory = "")
    {
        std::string canvas_name{name + std::string("_canvas_")};
        std::string canvas_path;
        if(directory.size() > 0)
        {
            canvas_path = directory + std::string("/");
        }
        canvas_path += filename;

        std::string log_mode_string;
        if(_log_mode_ == true) log_mode_string = std::string("_log");
        std::string full_path_eps{canvas_path + log_mode_string + std::string(".eps")};
        std::string full_path_png{canvas_path + log_mode_string + std::string(".png")};
        std::string full_path_pdf{canvas_path + log_mode_string + std::string(".pdf")};
        std::string full_path_C{canvas_path + log_mode_string + std::string(".C")};

        // init canvas
        TCanvas *c = new TCanvas(name.c_str(), "", _canvas_size_x_, _canvas_size_y_);
        if(_log_mode_ == true) c->SetLogy();

        // Set histogram axis title and label font and font size
        Iterator_t it{_histogram.begin()};
        for(; it != _histogram_.end(); ++ it)
        {
            
            // TODO: re-enable and decide on default value
            /*
            (*it)->Histogram()->GetXaxis()->SetTitleSize(15);
            (*it)->Histogram()->GetYaxis()->SetTitleSize(15);
            
            (*it)->Histogram()->GetXaxis()->SetTitleFont(43);
            (*it)->Histogram()->GetYaxis()->SetTitleFont(43);
            
            (*it)->Histogram()->GetXaxis()->SetLabelSize(15);
            (*it)->Histogram()->GetYaxis()->SetLabelSize(15);
            
            (*it)->Histogram()->GetXaxis()->SetLabelFont(43);
            (*it)->Histogram()->GetYaxis()->SetLabelFont(43);
            */

            // set colors
            std::size_t index{std::distance(_histogram_.cbegin(), it)};
            Int_t color{get_histogram_color(index, std::distance(_histogram_.begin(), _histogram_.end()))};
            
            it->second->Histogram()->SetLineColor(histogram_color);
            it->second->Histogram()->SetMarkerColor(histogram_color);
            
            // TODO: legend

            // draw with correct drawing option
            std::string draw_option{_draw_opt_};
            if(index > 0) draw_option += std::string("same");
            it->second->Histogram()->Draw(draw_option.c_str());


        }


        c->SaveAs(full_path_eps.c_str());
        c->SaveAs(full_path_png.c_str());
        c->SaveAs(full_path_pdf.c_str());
        c->SaveAs(full_path_C.c_str());

        delete c;


    }


    private:

    Int_t get_histogram_color(const std::size_t index, const std::size_t count)
    {
        if(count == 1)
        {
            return 4; // blue
        }
        else if(count == 2)
        {   
            if(index == 0) return 4; // blue
            else return 1; // black
        }
        /*
        else if(count == 3)
        {
            if(index == 0) return 2; // red
            else if(index == 1) return 3; // green
            else return 4; // blue
        }
        */
        else if(count <= 4)
        {
            if(index == 0) return 2; // red
            else if(index == 1) return 3; // green
            else if(index == 2) return 4; // blue
            else return 1; // black
        } 
        else if(count <= 6)
        {
            if(index == 0) return 2; // red
            else if(index == 1) return 3; // green
            else if(index == 2) return 4; // blue
            else if(index == 3) return 6; // magenta
            else if(index == 4) return 7; // cyan
            else return 1; // black
        }
        else if(count <= 10)
        {
            if(index == 0) return 1; // black
            else if(index == 1) return kOrange + 4; // brown
            else if(index == 2) return 2; // red
            else if(index == 3) return kOrange + 7; // orange
            else if(index == 4) return kOrange - 2; // yellow
            else if(index == 5) return 3; // green
            else if(index == 6) return 7; // cyan
            else if(index == 7) return 4; // blue
            else if(index == 8) return kViolet; // purple
            else if(index == 9) return kBlack + 2; // grey
        }
        else
        {
            if(index <= 15)
            {
                return (kBlue - 10) + index;
            }
            else
            {
                return (kViolet - 9) + index;
            }
        }


    }


    public:

    // add histogram which already exists
    // this mode no need to give a key as histogram is never accessed by the user
    void Add(const TH1* histogram)
    {
        std::size_t index{std::distance(_histogram_.begin(), _histogram_.end()};
        std::string key(std::to_string(index));
        
        Add(key, histogram);
    }

    void Add(const std::string& key, const TH1* histogram)
    {
        ConstIterator_t it{_histogram_.find(key)};
        if(it == _histogram_.end())
        {
            const TH1D* histo{histogram};
            histo->GetXaxis()->SetTitle(_x_axis_label_text_);
            histo->GetYaxis()->SetTitle(_y_axis_label_text_);
            _histogram_[key] = histo;
        }
        else
        {
            std::cerr << "[ ERROR ] : HistogramWrapper2::Add() -> key " << key << " exists in map" << std::endl;
        }
    }


    // add histogram
    void Add(const std::string& key, const Int_t n_bins_x, const Double_t x_low, const Double_t x_high)
    {
        // set line width
        // NOTE: do not set = set to default


        ConstIterator_t it{_histogram_.find(key)};
        if(it == _histogram_.end())
        {
            std::string histo_name{name + std::string("_") + key};
            TH1D* histo{new TH1D(histo_name.c_str(), "", n_bins_x, x_low, x_high)};
            histo->GetXaxis()->SetTitle(_x_axis_label_text_);
            histo->GetYaxis()->SetTitle(_y_axis_label_text_);
            _histogram_[key] = histo;
        }
        else
        {
            std::cerr << "[ ERROR ] : HistogramWrapper2::Add() -> key " << key << " exists in map" << std::endl;
        }
    }

    private:

    // name for histogram creation (base of names)
    std::string _name_;

    // axis label text
    std::string _x_axis_label_text_;
    std::string _y_axis_label_text_;

    // draw options - default for all histograms,
    // TODO: over-ride on per-histogram basis
    // option "same" automatically included for histograms after first is drawn
    std::string _draw_opt_;

    // canvas log mode
    bool _log_mode_;

    // default axis label font size can be changed here
    const TYPE _DEFAULT_AXIS_LABEL_FONT_SIZE_{value};
    TYPE _axis_label_font_size_;

    // default axis label font type can be changed here
    const TYPE _DEFAULT_AXIS_LABEL_FONT_{value};
    TYPE _axis_label_font_;

    // default axis number font size can be changed here
    const TYPE _DEFAULT_AXIS_NUMBER_FONT_SIZE_{value};
    TYPE _axis_number_font_size_;

    // default canvas size x and y
    const Int_t _DEFAULT_CANVAS_SIZE_X_{804}; // 800 + (800 - 796)
    const Int_t _DEFAULT_CANVAS_SIZE_Y_{628}; // 600 + (600 - 572)
    Int_t _canvas_size_x_;
    Int_t _canvas_size_y_;


    // y axis max over-ride
    bool _y_axis_max_override_;
    Double_t _y_axis_max_;


    // Histogram
    std::map<const std::string, TH1Data> _histogram_;
    typedef std::map<const std::string, TH1Data>::const_iterator ConstIterator_t;
    typedef std::map<const std::string, TH1Data>::iterator Iterator_t;

};



class HistogramProperties
{

    public:

    HistogramProperties(const Int_t n_bins_x, const Double_t low_x, const Double_t high_x)
        : draw_option{""}
        , name{""}
        , canvas_path{""}
        , n_bins_x{n_bins_x}
        , low_x{low_x}
        , high_x{high_x}
    {
    }

    HistogramProperties(const std::string& name, const Int_t n_bins_x, const Double_t low_x, const Double_t high_x)
        : draw_option{""}
        , name{name}
        , canvas_path{""}
        , n_bins_x{n_bins_x}
        , low_x{low_x}
        , high_x{high_x}
    {
    }

    HistogramProperties(const std::string& name, const std::string& canvas_path, const Int_t n_bins, const Double_t low_x, const Double_t high_x)
        : draw_option{""}
        , name{name}
        , canvas_path{canvas_path}
        , n_bins_x{n_bins_x}
        , low_x{low_x}
        , high_x{high_x}
    {
    }

    std::string draw_option;
    std::string name;
    std::string canvas_path;
    Int_t n_bins_x;
    Double_t low_x;
    Double_t high_x;

};



class HistogramWrapperFloat
{


    public:

    HistogramWrapperFloat(TFile* const tfile, const std::string& name, const std::string& canvas_path, const Int_t n_bins, const Float_t low, const Float_t high)
        //: _histogram_{new TH1F(name.c_str(), name.c_str(), n_bins, low, high)}
        : _histogram_{TH1F(name.c_str(), name.c_str(), n_bins, low, high)}
        , _histogram_properties_{HistogramProperties(name, canvas_path, n_bins, low, high)}
        , _tfile_{tfile}
    {
        Init();
    }

    HistogramWrapperFloat(TFile* const tfile, const HistogramProperties& histogram_properties)
        //: _histogram_{new TH1F(histogram_properties.name.c_str(), histogram_properties.name.c_str(), histogram_properties.n_bins_x, histogram_properties.low_x, histogram_properties.high_x)}
        : _histogram_{TH1F(histogram_properties.name.c_str(), histogram_properties.name.c_str(), histogram_properties.n_bins_x, histogram_properties.low_x, histogram_properties.high_x)}
        , _histogram_properties_{histogram_properties}
        , _tfile_{tfile}
    {

        /*
        std::cout << __func__ << "\n"\
                     "name=" << histogram_properties.name << "\n"\
                     "n_bins_x=" << histogram_properties.n_bins_x << "\n"\
                     "low_x=" << histogram_properties.low_x << "\n"\
                     "high_x=" << histogram_properties.high_x << "\n"\
                     "draw_option=" << histogram_properties.draw_option << "\n"\
                     "canvas_path=" << histogram_properties.canvas_path << std::endl;
        */

        //DEBUG_MESSAGE(function_debug_locals(__PRETTY_FUNCTION__, {{"name", histogram_properties.name}, {"n_bins_x", std::to_string(histogram_properties.n_bins_x)}, {"low_x", std::to_string(histogram_properties.low_x)}, {"high_x", std::to_string(histogram_properties.high_x)}}));
        DEBUG_MESSAGE(function_debug_locals(__PRETTY_FUNCTION__, "name", histogram_properties.name, "n_bins_x", histogram_properties.n_bins_x, "low_x", histogram_properties.low_x, "high_x", histogram_properties.high_x));


        Init();
    }

    HistogramWrapperFloat(const HistogramWrapperFloat& other) = default;

    ~HistogramWrapperFloat()
    {
        //_tfile_->cd();
        //_histogram_->Write();
        // Note: no longer write as file may have closed
        //Write();

        // decouple before pointer goes out of scope
        // TODO; doesn't work if class is copied
        // or there is more than one shared pointer reference
        //_histogram_->SetDirectory(0);
    }

    void Init()
    {
        //_histogram_->SetDirectory(0);
        //_histogram_->SetStats(0);
        _histogram_.SetStats(0);
    }

    void Canvas() //const
    {
        std::string full_path;
        if(_histogram_properties_.canvas_path.length() > 0)
        {
            full_path = _histogram_properties_.canvas_path + std::string("/");
        }
        full_path += _histogram_properties_.name;
        std::string full_path_png{full_path + std::string(".png")};
        std::string full_path_C{full_path + std::string(".C")};

        //std::cout << "full_path=" << full_path << std::endl;
        //std::cin.get();

        TCanvas *c = new TCanvas(_histogram_properties_.name.c_str(), _histogram_properties_.name.c_str(), 800, 600);
        
        //_histogram_->Draw(_histogram_properties_.draw_option.c_str());
        _histogram_.Draw(_histogram_properties_.draw_option.c_str());

        c->SaveAs(full_path_png.c_str());
        c->SaveAs(full_path_C.c_str());

        delete c;
    }

    void Write()
    {
        _tfile_->cd();
        //_histogram_->Write();
        _histogram_.Write();
    }

    //TH1F* Get()
    TH1F& Ref()
    {
        //return _histogram_.get();
        return _histogram_;
    }

    private:

    //std::shared_ptr<TH1F> _histogram_;
    TH1F _histogram_;
    //std::string _drawopt_;
    //std::string _name_;
    //std::string _canvas_path_;
    HistogramProperties _histogram_properties_;

    TFile *_tfile_;

};



class HistogramGroupProperties
{

    public:

    HistogramGroupProperties(const std::string& name_base, const std::string& canvas_path)
        : name_base{name_base}
        , canvas_path{canvas_path}
    {
    }


    std::string name_base;
    std::string canvas_path;

};



class HistogramGroupFloat
{


    public:


    HistogramGroupFloat(TFile* const tfile, const HistogramGroupProperties& histogram_group_properties, const HistogramProperties& default_histogram_properties)
        : _histogram_group_properties_{histogram_group_properties}
        , _default_histogram_properties_{default_histogram_properties}
        , _tfile_{tfile}
    {
    }

    HistogramGroupFloat(const HistogramGroupFloat& other) = default;

    std::string Add(const std::string& name, Int_t n_bins_x, Double_t low_x, Double_t high_x)
    {
        //DEBUG_MESSAGE(function_debug_arguments(__PRETTY_FUNCTION__, {{"name", name}, {"n_bins_x", std::to_string(n_bins_x)}, {"low_x", std::to_string(low_x)}, {"high_x", std::to_string(high_x)}}));
        //DEBUG_MESSAGE(function_debug_arguments(__PRETTY_FUNCTION__, "name", name, "n_bins_x", std::to_string(n_bins_x), "low_x", std::to_string(low_x), "high_x", std::to_string(high_x)));

        // initialize properties
        HistogramProperties histogram_properties{_default_histogram_properties_};
        histogram_properties.name = name;
        histogram_properties.n_bins_x = n_bins_x;
        histogram_properties.low_x = low_x;
        histogram_properties.high_x = high_x;

        // use the directory from the default group properties
        histogram_properties.canvas_path = _histogram_group_properties_.canvas_path;

        // create histogram wrapper object
        HistogramWrapperFloat next(_tfile_, histogram_properties);

        // add to container
        //_histogram_wrapper_.push_back(next);
        _histogram_wrapper_.insert({name, next});


        return name;
    }

    std::string Add(Int_t n_bins_x, Double_t low_x, Double_t high_x)
    {
        // create a new name
        std::string name{_histogram_group_properties_.name_base + to_string_fixed_width(_histogram_wrapper_.size(), 6)};
        Add(name, n_bins_x, low_x, high_x);
        return name;
    }

    void Canvas()
    {
        Iterator_t it{_histogram_wrapper_.begin()};
        for(; it != _histogram_wrapper_.end(); ++ it)
        {
            it->second.Canvas();    
        }
    }

    void Write()
    {
        Iterator_t it{_histogram_wrapper_.begin()};
        for(; it != _histogram_wrapper_.end(); ++ it)
        {
            it->second.Write();    
        }
    }

    /*
    HistogramWrapperFloat& Ref(const std::string& name)
    {
        return _histogram_wrapper_.at(name);
    }
    */

    TH1F& Ref(const std::string& name)
    {
        return _histogram_wrapper_.at(name).Ref();
    }


    private:

    //std::vector<HistogramWrapperFloat> _histogram_wrapper_;
    std::map<const std::string, HistogramWrapperFloat> _histogram_wrapper_;
    //typedef std::vector<HistogramWrapperFloat>::iterator Iterator_t;
    typedef std::map<const std::string, HistogramWrapperFloat>::iterator Iterator_t;
    HistogramGroupProperties _histogram_group_properties_;
    HistogramProperties _default_histogram_properties_;

    TFile* _tfile_;

};


#endif
