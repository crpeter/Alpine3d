#include <cstdint>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

using namespace std;

mongocxx::collection collection;

void getLayeringValues(bsoncxx::document::view weatherArr, vector<double>& density, vector<double>& viscosity, vector<double>& thickness, double& surfaceLayerHardness);

int findWeatherRecordInDocument(string expectedTimeStamp, int I, int J, string Cell_Lat, string Cell_Lon, vector<double>& density, vector<double>& viscosity, vector<double>& thickness, double& surfaceLayerHardness);

void getSpecialFriendValues(vector<double>& density, vector<double>& viscosity, vector<double>& thickness, int& SkiD, int& BootD, int& SkinD);

int updateCollection(
                      int weatherRecord,
                      int I,
                      int J,
                      std::string Cell_Lat,
                      std::string Cell_Lon,
                      double TA,
                      double VW,
                      double DW,
                      double VW_MAX,
                      double PSUM,
                      double PPH,
                      double ISWR,
                      double ISWR_B,
                      double HN,
                      double HNrho,
                      double HN24,
                      double HN72,
                      double SkiD,
                      double BootD,
                      double SkinD,
                      double SLH,
                      double FSL,
                      double TSS,
                      double HS,
                      vector<double> Qual,
                      vector<double> Th,
                      vector<double> Hd,
                      vector<double> Den,
                      vector<double> Te,
                      vector<double> LW,
                      vector<bool> DD,
                      vector<double> Visc,
                      vector<short unsigned int> Type
                      ) {
    
    std::string recordToUpdate = "Weather." + to_string(weatherRecord);
    
    //before updating the document, need to construct bson arrays
    using bsoncxx::builder::stream::array;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    
    auto QualArr = array{};
    auto ThArr = array{};
    auto HdArr = array{};
    auto DenArr = array{};
    auto TeArr = array{};
    auto LWArr = array{};
    auto DDArr = array{};
    auto ViscArr = array{};
    auto TypeArr = array{};
    
    for (int j = 0; j < Qual.size(); j++) {
        QualArr << Qual[j];
        ThArr << Th[j];
        HdArr << Hd[j];
        DenArr << Den[j];
        TeArr << Te[j];
        LWArr << LW[j];
        DDArr << DD[j];
        ViscArr << Visc[j];
        TypeArr << Type[j];
    }
    QualArr << close_array;
    ThArr << close_array;
    HdArr << close_array;
    DenArr << close_array;
    TeArr << close_array;
    LWArr << close_array;
    DDArr << close_array;
    ViscArr << close_array;
    TypeArr << close_array;
    
    // Update first document that matches filter
    bsoncxx::stdx::optional<mongocxx::result::update> result =
    collection.update_one(document{}
                          << "Location.I" << I
                          << "Location.J" << J
                          << "Location.Cell_Lat" << Cell_Lat
                          << "Location.Cell_Lon" << Cell_Lon
                          << finalize,
                          document{} << "$set"
                          << open_document
                          << recordToUpdate + ".TA" << TA
                          << recordToUpdate + ".VW" << VW
                          << recordToUpdate + ".DW" << DW
                          << recordToUpdate + ".VW_MAX" << VW_MAX
                          << recordToUpdate + ".PSUM" << PSUM
                          << recordToUpdate + ".PPH" << PPH
                          << recordToUpdate + ".ISWR" << ISWR
                          << recordToUpdate + ".ISWR_B" << ISWR_B
                          << recordToUpdate + ".HN" << HN
                          << recordToUpdate + ".HNrho" << HNrho
                          << recordToUpdate + ".HN24" << HN24
                          << recordToUpdate + ".HN72" << HN72
                          
                          << recordToUpdate + ".SkiD" << SkiD
                          << recordToUpdate + ".BootD" << BootD
                          << recordToUpdate + ".SkinD" << SkinD
                          
                          << recordToUpdate + ".SLH" << SLH
                          << recordToUpdate + ".FSL" << FSL
                          << recordToUpdate + ".TSS" << TSS
                          << recordToUpdate + ".HS" << HS
                          << close_document
                          
                          << "$push"
                          << open_document
                          << recordToUpdate + ".Layering.Qual"
                              << open_document
                              << "$each" << QualArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Th"
                              << open_document
                              << "$each" << ThArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Hd"
                              << open_document
                              << "$each" << HdArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Den"
                              << open_document
                              << "$each" << DenArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Te"
                              << open_document
                              << "$each" << TeArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.LW"
                              << open_document
                              << "$each" << LWArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.DD"
                              << open_document
                              << "$each" << DDArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Visc"
                              << open_document
                              << "$each" << ViscArr.view()
                              << close_document
                          << recordToUpdate + ".Layering.Type"
                              << open_document
                              << "$each" << TypeArr.view()
                              << close_document
                          << close_document
                          << finalize);
    if (result) {
        printf("updated: %d\n", result->modified_count());
    }
}

int findWeatherRecordInDocument(std::string expectedTimeStamp, int I, int J, std::string Cell_Lat, std::string Cell_Lon, vector<double> density, vector<double> viscosity, vector<double> thickness, double surfaceLayerHardness) {

    bsoncxx::stdx::optional<bsoncxx::document::value> optional_value_result =
    collection.find_one(
                              document{}
                                << "Location.I" << I
                                << "Location.J" << J
                                << "Location.Cell_Lat" << Cell_Lat
                                << "Location.Cell_Lon" << Cell_Lon
                                << finalize
                              );
    
    if(optional_value_result) {
        bsoncxx::document::view view = optional_value_result->view();
        // iterate over the elements in a bson document
        for (bsoncxx::document::element ele : view) {

            bsoncxx::stdx::string_view field_key = ele.key();

            if (ele.type() == bsoncxx::type::k_array) {
                
                int index = 0;
                bsoncxx::array::view weatherArr = ele.get_array().value;
                for (bsoncxx::array::element weatherEle : weatherArr) {

                    bsoncxx::document::view subarr = weatherEle.get_document().view();

                    for (bsoncxx::document::element e : subarr) {
                        if (e.type() == bsoncxx::type::k_utf8 && e.key().compare("timestamp") == 0) {
                            std::string currentTimeStamp = bsoncxx::string::to_string(e.get_value().get_utf8().value);
                            
                            if (currentTimeStamp.compare(expectedTimeStamp) == 0) {
                                cout << "found expected stamp: " << expectedTimeStamp << " at i: " << index << endl;
                                
                                // Get values from Layering array after finding correct timestamp
                                getLayeringValues(subarr, density, viscosity, thickness, surfaceLayerHardness);
                                return index;
                            }
                        }
                    }
                    ++index;
                }
            }
        }
    }
    return -1;
}

void getLayeringValues(bsoncxx::document::view selectedArr, vector<double>& density, vector<double>& viscosity, vector<double>& thickness, double& surfaceLayerHardness) {
    double newDens = density[0];
    double newVisc = viscosity[0];
    double newThick = thickness[0];
    density.pop_back();
    viscosity.pop_back();
    thickness.pop_back();
    for (bsoncxx::document::element e : selectedArr) {
        if (e.type() == bsoncxx::type::k_document && e.key().compare("Layering") == 0) {
            bsoncxx::document::view layerDoc = e.get_document().view();

            for (bsoncxx::document::element layerArr : layerDoc) {
                if (layerArr.key().compare("Th") == 0) {
                    cout << "Th:" << endl;
                    for (bsoncxx::array::element lEle : layerArr.get_value().get_array().value) {
                        thickness.push_back(lEle.get_value().get_double().value);
                        cout << to_string(lEle.get_value().get_double().value) << endl;
                    }
                    thickness.push_back(newThick);
                } else if (layerArr.key().compare("Den") == 0) {
                    cout << "Den:" << endl;
                    for (bsoncxx::array::element lEle : layerArr.get_value().get_array().value) {
                        density.push_back(lEle.get_value().get_double().value);
                        cout << to_string(lEle.get_value().get_double().value) << endl;
                    }
                    density.push_back(newDens);
                } else if (layerArr.key().compare("Visc") == 0) {
                    cout << "Vis:" << endl;
                    for (bsoncxx::array::element lEle : layerArr.get_value().get_array().value) {
                        viscosity.push_back(lEle.get_value().get_double().value);
                        cout << to_string(lEle.get_value().get_double().value) << endl;
                    }
                    viscosity.push_back(newVisc);
                } else if (layerArr.key().compare("Hd") == 0) {
                    cout << "Hd:" << endl;
                    int help = 0;
                    for (bsoncxx::array::element lEle : layerArr.get_value().get_array().value) {
                        if (help == 0) {
                            cout << to_string(lEle.get_value().get_double().value) << endl;
                            surfaceLayerHardness = lEle.get_value().get_double().value;
                        }
                        ++help;
                    }
                }
            }
        }
    }
}

void getSpecialFriendValues(vector<double>& density, vector<double>& viscosity, vector<double>& thickness, double& quality, int& SkiD, int& BootD, int& SkinD, double& firmest) {
    double skipen = 60.0 * 1.76 * 0.130 * 25.0 * 25.0;
    double bootpen = 60.0 * 0.290 * 0.130 * 25.0 * 25.0;
    double skinpen = 60.0 * 1.76 * 0.130 * 5.0 * 5.0;
    //printf("%6f %6f %6f\n", skipen, bootpen, skinpen);
    double cumThickness = 0.0;
    firmest = 0.0;
    for (int i = 0; i < density.size() && i < viscosity.size() && i < thickness.size(); i++) {
        cumThickness += density[density.size() - 1 - i] * viscosity[density.size() - 1 - i] * thickness[density.size() - 1 - i];
        if (density[density.size() - 1 - i] * viscosity[density.size() - 1 - i] * thickness[density.size() - 1 - i] > firmest) {
            firmest = density[density.size() - 1 - i] * viscosity[density.size() - 1 - i] * thickness[density.size() - 1 - i];
        }
        if (skipen >=  cumThickness) {
            SkiD = i;
        }
        if (bootpen >= cumThickness) {
            BootD = i;
        }
        if (skinpen >= cumThickness) {
            SkinD = i;
        }
        cout << "Cum Thickness: " << cumThickness << endl;
    }
}

int main(int, char**) {
    
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb+srv://petercodypeter:pooptart@testcluster-u08oa.mongodb.net/test?retryWrites=true&w=majority"}};

    mongocxx::collection sampleCollection = conn["sample_alpine"]["sample"];

    collection = sampleCollection;

    
    // Test values
    string timestamp = "2019-06-05T01:00:00";
    int I = 110;
    int J = 2;
    string Cell_Lat = "39.4";
    string Cell_Lon = "-106.1";
    
    double density = 15.69; double visc = 10.69; double thick = 11.69;
    double airTemp = 69.6969; double windSpeed = 20.69; double windDirection = 180.69;
    double gustSpeed = 5.69; double precipAmount = 0.69; double precipPhase = 0.69;
    double iswr = 10.69; double iswr_b = 10.69; double newSnow1 = 1.69; double newSnow24 = 24.69;
    double newSnow72 = 72.69; double newSnowDens1 = 2.69;
    
    double surfaceTemp = 45.69; double snowHeight = 36.69; double temp = 69.69;
    double layerLiquidWater = 0.69; double layerResidWater = 0.69; bool dendricity = 1;
    double hardness = 11.69; string grainType = "Good";
    // End test values
    
    
    vector<double> densityVec;
    vector<double> viscosityVec;
    vector<double> thicknessVec;
    densityVec.push_back(density); viscosityVec.push_back(visc); thicknessVec.push_back(thick);
    
    
    vector<double> Qual; Qual.push_back(10.33);
    vector<double> Hd; Hd.push_back(10.33);
    vector<double> Te; Te.push_back(10.33);
    vector<double> LW; LW.push_back(10.33);
    vector<bool> DD; DD.push_back(false);
    vector<short unsigned int> Type; Qual.push_back(3);
    
    double quality = log(1 + density + visc);
    int SkiD; int BootD; int SkinD; double firmest; double surfaceLayerHardness;
    int index = findWeatherRecordInDocument(timestamp, I, J, Cell_Lat, Cell_Lon, densityVec, viscosityVec, thicknessVec, surfaceLayerHardness);
    
    if (index != -1) {
        // Index of timestamp found
        getSpecialFriendValues(densityVec, viscosityVec, thicknessVec, quality, SkiD, BootD, SkinD, firmest);
        printf("den: %lu, visc: %lu, thick: %lu\n", densityVec.size(), viscosityVec.size(), thicknessVec.size());
        printf("SkiD: %d, BootD: %d, SkindD: %d\n", SkiD, BootD, SkinD);
//        updateCollection(index, I, J, Cell_Lat, Cell_Lon,
//                         /*TA*/     airTemp,
//                         /*VW*/     windSpeed,
//                         /*DW*/     windDirection,
//                         /*VW_MAX*/ gustSpeed,
//                         /*PSUM*/   precipAmount,
//                         /*PPH*/    precipPhase,
//                         /*ISWR*/   iswr,
//                         /*ISWR_B*/ iswr_b,
//                         /*HN*/     newSnow1,
//                         /*HNrho*/  newSnowDens1,
//                         /*HN24*/   newSnow24,
//                         /*HN72*/   newSnow72,
//                         /*SkiD*/   SkiD,
//                         /*BootD*/  BootD,
//                         /*SkinD*/  SkinD,
//                         /*SLH*/    surfaceLayerHardness,
//                         /*FSL*/    firmest,
//                         /*TSS*/    surfaceTemp,
//                         /*HS*/     snowHeight,
//                         /*Qual*/   Qual,
//                         /*TH*/     thicknessVec,
//                         /*Hd*/     Hd,
//                         /*Den*/    densityVec,
//                         /*Te*/     Te,
//                         /*LW*/     LW,
//                         /*DD*/     DD,
//                         /*Visc*/   viscosityVec,
//                         /*Type*/   Type
//                         );
    } else {
        // No matching Timestamp found
        
    }
}
