#ifndef HISTOGRAMWRAPPER_HPP
#define HISTOGRAMWRAPPER_HPP


#include "numerical_string_format.hpp"
#include "function_debug.hpp"


#include <memory>
#include <map>


#include <TH1.h>
#include <TFile.h>
#include <TCanvas.h>



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
