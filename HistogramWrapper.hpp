#ifndef HISTOGRAMWRAPPER_HPP
#define HISTOGRAMWRAPPER_HPP


#include "numerical_string_format.hpp"


class HistogramProperties
{

    public:

    HistogramProperties(const Int_t n_bins_x, const Double_t low_x, const Double_t high_x)
        : this->draw_option{""}
        , this->name{""}
        , this->canvas_path{""}
        , this->n_bins_x{n_bins_x}
        , this->low_x{low_x}
        , this->high_x{high_x}
    {
    }

    HistogramProperties(const std::string& name, const Int_t n_bins_x, const Double_t low_x, const Double_t high_x)
        : this->draw_option{""}
        , this->name{name}
        , this->canvas_path{""}
        , this->n_bins_x{n_bins_x}
        , this->low_x{low_x}
        , this->high_x{high_x}
    {
    }

    HistogramProperties(const std::string& name, const std::string& canvas_path, const Int_t n_bins, const Double_t low_x, const Double_t high_x)
        : this->draw_option{""}
        , this->name{name}
        , this->canvas_path{canvas_path}
        , this->n_bins{n_bins}
        , this->low_x{low_x}
        , this->high_x{high_x}
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

    HistogramWrapper(const std::string& name, const std::string& canvas_path, const Int_t n_bins, const Float_t low, const Float_t high)
        : _histogram_{new TH1F(name.c_str(), name.c_str(), n_bins, low, high)}
        , _histogram_properties_{histogram_properties}
    {
        Init();
    }

    HistogramWrapper(const HistogramProperties& histogram_properties)
        : _histogram_{new TH1F(histogram_properties.name.c_str(), histogram_properties.name.c_str(), histogram_properties.n_bins, histogram_properties.low, histogram_properties.high)}
        , _histogram_properties_{histogram_properties}
    {
        Init();
    }

    ~HistogramWrapper()
    {
        _file_->Cd();
        _histogram_->Write();
    }

    void Init()
    {
        _histogram_->SetStats(0);
    }

    void Canvas() const
    {
        std::string full_path{_histogram_properties_._canvas_path_ + std::string("/") + _name_};
        std::string full_path_png{_histogram_properties_.full_path + std::string(".png")};
        std::string full_path_C{_histogram_properties_.full_path + std::string(".C")};

        TCanvas *c = new TCanvas(_histogram_properties_._name_.c_str(), _histogram_properties_._name_.c_str(), 800, 600);
        
        _histogram_->Draw(_histogram_properties_._drawopt_.c_str());

        c->SaveAs(full_path_png.c_str());
        c->SaveAs(full_path_C.c_str());

        delete c;
    }

    TH1F* Get()
    {
        return _histogram_->get();
    }

    private:

    std::unique_ptr<TH1F> _histogram_;
    //std::string _drawopt_;
    //std::string _name_;
    //std::string _canvas_path_;
    HistogramProperties _histogram_properties_

    TFile &_file_;

};


class HistogramGroupProperties
{

    public:

    HistogramGroupProperties(const std::string& name_base, const std::string& canvas_path)
        : this->name_base{name_base}
        , this->canvas_path{canvas_path}
    {
    }


    std::string name_base;
    std::string canvas_path;

}

class HistogramGroupFloat
{


    public:

    HistogramGroupFloat(const HistogramGroupProperties& histogram_group_properties, const HistogramProperties& default_histogram_properties)
        : _histogram_group_properties_{histogram_group_properties}
        , _default_histogram_properties_{default_histogram_properties}
    {
    }

    void Add(const std::string& name, Int_t n_bins_x, Double_t low_x, Double_t high_x)
    {
        // initialize properties
        HistogramProperties histogram_properties{_default_histogram_properties_};
        histogram_properties.name = name;
        histogram_properties.n_bins_x = n_bins_x;
        histogram_properties.low_x = low_x;
        histogram_properties.high_x = high_x;

        // create histogram wrapper object
        HistogramWrapper next(histogram_properties);

        // add to container
        _histogram_group_.push_back(next);
    }

    void Add(Int_t n_bins_x, Double_t low_x, Double_t high_x)
    {
        // create a new name
        std::string name{name_base + to_string_fixed_width(_histogram_wrapper_.size(), 6};
        Add(name, n_bins_x, low_x, high_x);
    }

    void Canvas()
    {
        Iterator_t it{_histogram_wrapper_.begin()};
        for(; it != _histogram_wrapper_.end(); ++ it)
        {
            it->Canvas();    
        }
    }

    void Write()
    {
        Iterator_t it{_histogram_wrapper_.begin()};
        for(; it != _histogram_wrapper_.end(); ++ it)
        {
            it->Write();    
        }
    }


    private:

    std::vector<HistogramWrapperFloat> _histogram_wrapper_;
    typedef std::vector<HistogramWrapperFloat>::iterator Iterator_t;
    HistogramGroupProperties _histogram_group_properties_;
    HistogramProperties _default_histogram_properties_;

};

#endif
