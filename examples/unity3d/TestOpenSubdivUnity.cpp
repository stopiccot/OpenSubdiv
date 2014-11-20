extern "C" {
    void Initialize(bool enableCodegen);
    void SetVertices(float* vertices, int vertexCount);
    void AddFace(int* faceVertices, int vertexCount);
    void Subdivide();
    void GetSubdividedTriangles(int** triangles, int* triangleCount);
    void GetSubdividedVertices(float** vertices, int* vertexCount);
}

#include <vector>

int main() {
	Initialize(false);

	{ int i[] = {62,19,10,61}; AddFace(i, 4); }
{ int i[] = {22,21,6,5}; AddFace(i, 4); }
{ int i[] = {23,22,5,4}; AddFace(i, 4); }
{ int i[] = {23,4,15,16}; AddFace(i, 4); }
{ int i[] = {43,8,18,42}; AddFace(i, 4); }
{ int i[] = {15,13,17,16}; AddFace(i, 4); }
{ int i[] = {20,7,6,21}; AddFace(i, 4); }
{ int i[] = {16,17,12,14}; AddFace(i, 4); }
{ int i[] = {18,9,41,42}; AddFace(i, 4); }
{ int i[] = {63,32,19,62}; AddFace(i, 4); }
{ int i[] = {21,0,1,20}; AddFace(i, 4); }
{ int i[] = {2,0,21,22}; AddFace(i, 4); }
{ int i[] = {3,2,22,23}; AddFace(i, 4); }
{ int i[] = {3,23,16,14}; AddFace(i, 4); }
{ int i[] = {55,6,7,44}; AddFace(i, 4); }
{ int i[] = {24,11,38,39}; AddFace(i, 4); }
{ int i[] = {27,24,39,40}; AddFace(i, 4); }
{ int i[] = {1,0,2,25}; AddFace(i, 4); }
{ int i[] = {26,25,2,3}; AddFace(i, 4); }
{ int i[] = {9,27,40,41}; AddFace(i, 4); }
{ int i[] = {34,35,10,19}; AddFace(i, 4); }
{ int i[] = {28,24,27,29}; AddFace(i, 4); }
{ int i[] = {29,27,9,31}; AddFace(i, 4); }
{ int i[] = {30,11,24,28}; AddFace(i, 4); }
{ int i[] = {31,9,18,8}; AddFace(i, 4); }
{ int i[] = {26,3,14,12}; AddFace(i, 4); }
{ int i[] = {1,38,32,63}; AddFace(i, 4); }
{ int i[] = {35,34,36,37}; AddFace(i, 4); }
{ int i[] = {36,34,19,32}; AddFace(i, 4); }
{ int i[] = {11,30,37,36}; AddFace(i, 4); }
{ int i[] = {35,37,33,10}; AddFace(i, 4); }
{ int i[] = {11,36,32,38}; AddFace(i, 4); }
{ int i[] = {25,39,38,1}; AddFace(i, 4); }
{ int i[] = {40,39,25,26}; AddFace(i, 4); }
{ int i[] = {41,40,26,12}; AddFace(i, 4); }
{ int i[] = {42,41,12,17}; AddFace(i, 4); }
{ int i[] = {13,43,42,17}; AddFace(i, 4); }
{ int i[] = {53,46,45,52}; AddFace(i, 4); }
{ int i[] = {60,61,10,33}; AddFace(i, 4); }
{ int i[] = {57,58,28,29}; AddFace(i, 4); }
{ int i[] = {56,43,13,47}; AddFace(i, 4); }
{ int i[] = {49,44,46,53}; AddFace(i, 4); }
{ int i[] = {15,54,47,13}; AddFace(i, 4); }
{ int i[] = {31,56,57,29}; AddFace(i, 4); }
{ int i[] = {28,58,59,30}; AddFace(i, 4); }
{ int i[] = {33,59,44,60}; AddFace(i, 4); }
{ int i[] = {5,50,51,4}; AddFace(i, 4); }
{ int i[] = {47,48,52,45}; AddFace(i, 4); }
{ int i[] = {50,49,48,51}; AddFace(i, 4); }
{ int i[] = {49,53,52,48}; AddFace(i, 4); }
{ int i[] = {4,51,54,15}; AddFace(i, 4); }
{ int i[] = {47,54,51,48}; AddFace(i, 4); }
{ int i[] = {50,55,44,49}; AddFace(i, 4); }
{ int i[] = {5,6,55,50}; AddFace(i, 4); }
{ int i[] = {56,47,45,57}; AddFace(i, 4); }
{ int i[] = {45,46,58,57}; AddFace(i, 4); }
{ int i[] = {59,58,46,44}; AddFace(i, 4); }
{ int i[] = {56,31,8,43}; AddFace(i, 4); }
{ int i[] = {37,30,59,33}; AddFace(i, 4); }
{ int i[] = {44,7,61,60}; AddFace(i, 4); }
{ int i[] = {20,62,61,7}; AddFace(i, 4); }
{ int i[] = {1,63,62,20}; AddFace(i, 4); }
{ int i[] = {67,65,107,106}; AddFace(i, 4); }
{ int i[] = {86,85,102,101}; AddFace(i, 4); }
{ int i[] = {71,69,95,94}; AddFace(i, 4); }
{ int i[] = {65,71,94,107}; AddFace(i, 4); }
{ int i[] = {65,67,69,71}; AddFace(i, 4); }
{ int i[] = {66,64,70,68}; AddFace(i, 4); }
{ int i[] = {73,67,106,105}; AddFace(i, 4); }
{ int i[] = {69,67,73,74}; AddFace(i, 4); }
{ int i[] = {69,74,96,95}; AddFace(i, 4); }
{ int i[] = {66,68,75,72}; AddFace(i, 4); }
{ int i[] = {104,77,109,108}; AddFace(i, 4); }
{ int i[] = {77,78,110,109}; AddFace(i, 4); }
{ int i[] = {78,97,111,110}; AddFace(i, 4); }
{ int i[] = {79,76,113,112}; AddFace(i, 4); }
{ int i[] = {81,77,104,103}; AddFace(i, 4); }
{ int i[] = {78,77,81,82}; AddFace(i, 4); }
{ int i[] = {89,91,99,98}; AddFace(i, 4); }
{ int i[] = {76,79,83,80}; AddFace(i, 4); }
{ int i[] = {85,81,103,102}; AddFace(i, 4); }
{ int i[] = {82,81,85,86}; AddFace(i, 4); }
{ int i[] = {91,93,100,99}; AddFace(i, 4); }
{ int i[] = {80,83,87,84}; AddFace(i, 4); }
{ int i[] = {78,89,98,97}; AddFace(i, 4); }
{ int i[] = {83,79,88,90}; AddFace(i, 4); }
{ int i[] = {78,82,91,89}; AddFace(i, 4); }
{ int i[] = {87,83,90,92}; AddFace(i, 4); }
{ int i[] = {93,86,101,100}; AddFace(i, 4); }
{ int i[] = {82,86,93,91}; AddFace(i, 4); }
{ int i[] = {68,70,94,95}; AddFace(i, 4); }
{ int i[] = {75,68,95,96}; AddFace(i, 4); }
{ int i[] = {97,79,112,111}; AddFace(i, 4); }
{ int i[] = {88,79,97,98}; AddFace(i, 4); }
{ int i[] = {90,88,98,99}; AddFace(i, 4); }
{ int i[] = {92,90,99,100}; AddFace(i, 4); }
{ int i[] = {87,92,100,101}; AddFace(i, 4); }
{ int i[] = {84,87,101,102}; AddFace(i, 4); }
{ int i[] = {80,84,102,103}; AddFace(i, 4); }
{ int i[] = {76,80,103,104}; AddFace(i, 4); }
{ int i[] = {76,104,108,113}; AddFace(i, 4); }
{ int i[] = {66,72,105,106}; AddFace(i, 4); }
{ int i[] = {64,66,106,107}; AddFace(i, 4); }
{ int i[] = {70,64,107,94}; AddFace(i, 4); }
{ int i[] = {108,109,114,119}; AddFace(i, 4); }
{ int i[] = {109,110,115,114}; AddFace(i, 4); }
{ int i[] = {110,111,116,115}; AddFace(i, 4); }
{ int i[] = {111,112,117,116}; AddFace(i, 4); }
{ int i[] = {112,113,118,117}; AddFace(i, 4); }
{ int i[] = {113,108,119,118}; AddFace(i, 4); }
{ int i[] = {74,73,114,115}; AddFace(i, 4); }
{ int i[] = {96,74,115,116}; AddFace(i, 4); }
{ int i[] = {75,96,116,117}; AddFace(i, 4); }
{ int i[] = {72,75,117,118}; AddFace(i, 4); }
{ int i[] = {105,72,118,119}; AddFace(i, 4); }
{ int i[] = {73,105,119,114}; AddFace(i, 4); }
{ int i[] = {508,120,531,530}; AddFace(i, 4); }
{ int i[] = {280,522,528,527}; AddFace(i, 4); }
{ int i[] = {514,515,517,516}; AddFace(i, 4); }
{ int i[] = {124,125,256,257}; AddFace(i, 4); }
{ int i[] = {516,517,519,518}; AddFace(i, 4); }
{ int i[] = {125,239,255,256}; AddFace(i, 4); }
{ int i[] = {126,127,546,329}; AddFace(i, 4); }
{ int i[] = {253,126,329,328}; AddFace(i, 4); }
{ int i[] = {126,250,520,513}; AddFace(i, 4); }
{ int i[] = {128,129,131,130}; AddFace(i, 4); }
{ int i[] = {130,131,133,132}; AddFace(i, 4); }
{ int i[] = {132,133,135,134}; AddFace(i, 4); }
{ int i[] = {134,135,137,136}; AddFace(i, 4); }
{ int i[] = {222,134,140,225}; AddFace(i, 4); }
{ int i[] = {136,137,139,138}; AddFace(i, 4); }
{ int i[] = {138,139,141,140}; AddFace(i, 4); }
{ int i[] = {140,141,143,142}; AddFace(i, 4); }
{ int i[] = {225,140,146,148}; AddFace(i, 4); }
{ int i[] = {142,143,145,144}; AddFace(i, 4); }
{ int i[] = {144,145,147,146}; AddFace(i, 4); }
{ int i[] = {146,147,149,148}; AddFace(i, 4); }
{ int i[] = {148,149,151,150}; AddFace(i, 4); }
{ int i[] = {150,152,296,456}; AddFace(i, 4); }
{ int i[] = {150,151,294,152}; AddFace(i, 4); }
{ int i[] = {154,242,298,303}; AddFace(i, 4); }
{ int i[] = {152,292,153,296}; AddFace(i, 4); }
{ int i[] = {293,154,303,302}; AddFace(i, 4); }
{ int i[] = {155,156,181,182}; AddFace(i, 4); }
{ int i[] = {155,182,279,276}; AddFace(i, 4); }
{ int i[] = {156,155,298,242}; AddFace(i, 4); }
{ int i[] = {157,263,499,497}; AddFace(i, 4); }
{ int i[] = {157,180,282,281}; AddFace(i, 4); }
{ int i[] = {241,159,371,370}; AddFace(i, 4); }
{ int i[] = {322,246,369,368}; AddFace(i, 4); }
{ int i[] = {159,161,372,371}; AddFace(i, 4); }
{ int i[] = {203,204,382,381}; AddFace(i, 4); }
{ int i[] = {341,160,382,204}; AddFace(i, 4); }
{ int i[] = {161,338,373,372}; AddFace(i, 4); }
{ int i[] = {336,203,381,380}; AddFace(i, 4); }
{ int i[] = {163,165,375,374}; AddFace(i, 4); }
{ int i[] = {213,214,379,378}; AddFace(i, 4); }
{ int i[] = {167,376,486,485}; AddFace(i, 4); }
{ int i[] = {377,212,489,488}; AddFace(i, 4); }
{ int i[] = {170,168,376,287}; AddFace(i, 4); }
{ int i[] = {168,170,269,211}; AddFace(i, 4); }
{ int i[] = {287,169,401,400}; AddFace(i, 4); }
{ int i[] = {269,170,399,398}; AddFace(i, 4); }
{ int i[] = {171,173,285,286}; AddFace(i, 4); }
{ int i[] = {172,174,267,268}; AddFace(i, 4); }
{ int i[] = {170,287,400,399}; AddFace(i, 4); }
{ int i[] = {285,173,495,494}; AddFace(i, 4); }
{ int i[] = {267,174,493,492}; AddFace(i, 4); }
{ int i[] = {174,172,286,285}; AddFace(i, 4); }
{ int i[] = {175,177,283,284}; AddFace(i, 4); }
{ int i[] = {176,178,265,266}; AddFace(i, 4); }
{ int i[] = {174,285,494,493}; AddFace(i, 4); }
{ int i[] = {179,282,332,331}; AddFace(i, 4); }
{ int i[] = {479,264,480,481}; AddFace(i, 4); }
{ int i[] = {178,176,284,283}; AddFace(i, 4); }
{ int i[] = {180,157,497,503}; AddFace(i, 4); }
{ int i[] = {282,180,333,332}; AddFace(i, 4); }
{ int i[] = {182,181,545,544}; AddFace(i, 4); }
{ int i[] = {182,275,278,279}; AddFace(i, 4); }
{ int i[] = {245,183,300,299}; AddFace(i, 4); }
{ int i[] = {184,153,243,244}; AddFace(i, 4); }
{ int i[] = {183,291,301,300}; AddFace(i, 4); }
{ int i[] = {290,185,532,288}; AddFace(i, 4); }
{ int i[] = {232,186,532,533}; AddFace(i, 4); }
{ int i[] = {297,262,458,457}; AddFace(i, 4); }
{ int i[] = {388,387,534,533}; AddFace(i, 4); }
{ int i[] = {387,386,535,534}; AddFace(i, 4); }
{ int i[] = {188,190,217,216}; AddFace(i, 4); }
{ int i[] = {188,216,233,232}; AddFace(i, 4); }
{ int i[] = {386,384,536,535}; AddFace(i, 4); }
{ int i[] = {190,192,221,217}; AddFace(i, 4); }
{ int i[] = {221,192,359,220}; AddFace(i, 4); }
{ int i[] = {344,194,361,360}; AddFace(i, 4); }
{ int i[] = {194,195,362,361}; AddFace(i, 4); }
{ int i[] = {195,340,363,362}; AddFace(i, 4); }
{ int i[] = {236,197,365,364}; AddFace(i, 4); }
{ int i[] = {197,199,366,365}; AddFace(i, 4); }
{ int i[] = {198,200,226,227}; AddFace(i, 4); }
{ int i[] = {198,227,234,235}; AddFace(i, 4); }
{ int i[] = {199,201,367,366}; AddFace(i, 4); }
{ int i[] = {200,202,215,226}; AddFace(i, 4); }
{ int i[] = {247,248,324,323}; AddFace(i, 4); }
{ int i[] = {204,203,392,391}; AddFace(i, 4); }
{ int i[] = {356,391,357,484}; AddFace(i, 4); }
{ int i[] = {263,257,470,469}; AddFace(i, 4); }
{ int i[] = {207,208,266,265}; AddFace(i, 4); }
{ int i[] = {214,213,395,394}; AddFace(i, 4); }
{ int i[] = {209,267,492,491}; AddFace(i, 4); }
{ int i[] = {212,396,490,489}; AddFace(i, 4); }
{ int i[] = {209,210,268,267}; AddFace(i, 4); }
{ int i[] = {212,211,397,396}; AddFace(i, 4); }
{ int i[] = {211,269,398,397}; AddFace(i, 4); }
{ int i[] = {324,248,354,353}; AddFace(i, 4); }
{ int i[] = {226,215,352,351}; AddFace(i, 4); }
{ int i[] = {229,228,348,347}; AddFace(i, 4); }
{ int i[] = {228,234,349,348}; AddFace(i, 4); }
{ int i[] = {230,229,347,346}; AddFace(i, 4); }
{ int i[] = {229,196,339,228}; AddFace(i, 4); }
{ int i[] = {219,196,229,230}; AddFace(i, 4); }
{ int i[] = {219,230,346,345}; AddFace(i, 4); }
{ int i[] = {222,225,330,252}; AddFace(i, 4); }
{ int i[] = {227,226,351,350}; AddFace(i, 4); }
{ int i[] = {234,227,350,349}; AddFace(i, 4); }
{ int i[] = {248,237,355,354}; AddFace(i, 4); }
{ int i[] = {256,255,472,471}; AddFace(i, 4); }
{ int i[] = {251,513,518,519}; AddFace(i, 4); }
{ int i[] = {239,250,254,255}; AddFace(i, 4); }
{ int i[] = {240,237,248,247}; AddFace(i, 4); }
{ int i[] = {246,241,370,369}; AddFace(i, 4); }
{ int i[] = {243,270,271,272}; AddFace(i, 4); }
{ int i[] = {244,243,272,273}; AddFace(i, 4); }
{ int i[] = {274,275,543,542}; AddFace(i, 4); }
{ int i[] = {255,254,473,472}; AddFace(i, 4); }
{ int i[] = {254,327,462,473}; AddFace(i, 4); }
{ int i[] = {250,126,253,254}; AddFace(i, 4); }
{ int i[] = {258,259,272,271}; AddFace(i, 4); }
{ int i[] = {259,260,273,272}; AddFace(i, 4); }
{ int i[] = {260,259,262,297}; AddFace(i, 4); }
{ int i[] = {261,274,542,541}; AddFace(i, 4); }
{ int i[] = {258,271,465,464}; AddFace(i, 4); }
{ int i[] = {263,469,500,499}; AddFace(i, 4); }
{ int i[] = {264,479,506,505}; AddFace(i, 4); }
{ int i[] = {270,155,276,277}; AddFace(i, 4); }
{ int i[] = {271,270,466,465}; AddFace(i, 4); }
{ int i[] = {270,277,467,466}; AddFace(i, 4); }
{ int i[] = {277,276,304,305}; AddFace(i, 4); }
{ int i[] = {277,305,468,467}; AddFace(i, 4); }
{ int i[] = {279,278,478,476}; AddFace(i, 4); }
{ int i[] = {152,288,289,292}; AddFace(i, 4); }
{ int i[] = {291,293,302,301}; AddFace(i, 4); }
{ int i[] = {288,152,294,290}; AddFace(i, 4); }
{ int i[] = {153,184,295,296}; AddFace(i, 4); }
{ int i[] = {254,253,328,327}; AddFace(i, 4); }
{ int i[] = {318,316,461,460}; AddFace(i, 4); }
{ int i[] = {155,270,243,298}; AddFace(i, 4); }
{ int i[] = {184,244,299,300}; AddFace(i, 4); }
{ int i[] = {289,184,300,301}; AddFace(i, 4); }
{ int i[] = {292,289,301,302}; AddFace(i, 4); }
{ int i[] = {153,292,302,303}; AddFace(i, 4); }
{ int i[] = {243,153,303,298}; AddFace(i, 4); }
{ int i[] = {305,304,496,498}; AddFace(i, 4); }
{ int i[] = {304,306,502,496}; AddFace(i, 4); }
{ int i[] = {468,305,498,501}; AddFace(i, 4); }
{ int i[] = {477,307,504,507}; AddFace(i, 4); }
{ int i[] = {308,309,315,314}; AddFace(i, 4); }
{ int i[] = {309,310,312,315}; AddFace(i, 4); }
{ int i[] = {311,313,475,474}; AddFace(i, 4); }
{ int i[] = {311,308,314,313}; AddFace(i, 4); }
{ int i[] = {313,279,476,475}; AddFace(i, 4); }
{ int i[] = {276,279,313,314}; AddFace(i, 4); }
{ int i[] = {304,276,314,315}; AddFace(i, 4); }
{ int i[] = {306,304,315,312}; AddFace(i, 4); }
{ int i[] = {275,182,544,543}; AddFace(i, 4); }
{ int i[] = {224,223,317,316}; AddFace(i, 4); }
{ int i[] = {218,224,316,318}; AddFace(i, 4); }
{ int i[] = {233,218,318,232}; AddFace(i, 4); }
{ int i[] = {320,261,541,540}; AddFace(i, 4); }
{ int i[] = {321,258,464,463}; AddFace(i, 4); }
{ int i[] = {201,322,368,367}; AddFace(i, 4); }
{ int i[] = {215,202,323,324}; AddFace(i, 4); }
{ int i[] = {215,324,353,352}; AddFace(i, 4); }
{ int i[] = {325,249,326,539}; AddFace(i, 4); }
{ int i[] = {327,321,463,462}; AddFace(i, 4); }
{ int i[] = {252,321,327,328}; AddFace(i, 4); }
{ int i[] = {222,252,328,329}; AddFace(i, 4); }
{ int i[] = {252,259,258,321}; AddFace(i, 4); }
{ int i[] = {259,252,330,262}; AddFace(i, 4); }
{ int i[] = {262,330,459,458}; AddFace(i, 4); }
{ int i[] = {124,257,263,527}; AddFace(i, 4); }
{ int i[] = {257,256,471,470}; AddFace(i, 4); }
{ int i[] = {203,336,393,392}; AddFace(i, 4); }
{ int i[] = {283,177,331,332}; AddFace(i, 4); }
{ int i[] = {178,283,332,333}; AddFace(i, 4); }
{ int i[] = {265,178,333,334}; AddFace(i, 4); }
{ int i[] = {207,265,334,335}; AddFace(i, 4); }
{ int i[] = {336,214,394,393}; AddFace(i, 4); }
{ int i[] = {214,336,380,379}; AddFace(i, 4); }
{ int i[] = {338,163,374,373}; AddFace(i, 4); }
{ int i[] = {340,236,364,363}; AddFace(i, 4); }
{ int i[] = {234,228,339,235}; AddFace(i, 4); }
{ int i[] = {237,341,356,355}; AddFace(i, 4); }
{ int i[] = {206,205,483,482}; AddFace(i, 4); }
{ int i[] = {264,206,482,480}; AddFace(i, 4); }
{ int i[] = {193,344,360,359}; AddFace(i, 4); }
{ int i[] = {221,220,345,346}; AddFace(i, 4); }
{ int i[] = {217,221,346,347}; AddFace(i, 4); }
{ int i[] = {216,217,347,348}; AddFace(i, 4); }
{ int i[] = {233,216,348,349}; AddFace(i, 4); }
{ int i[] = {218,233,349,350}; AddFace(i, 4); }
{ int i[] = {224,218,350,351}; AddFace(i, 4); }
{ int i[] = {223,224,351,352}; AddFace(i, 4); }
{ int i[] = {325,223,352,353}; AddFace(i, 4); }
{ int i[] = {249,325,353,354}; AddFace(i, 4); }
{ int i[] = {238,249,354,355}; AddFace(i, 4); }
{ int i[] = {205,238,355,356}; AddFace(i, 4); }
{ int i[] = {205,356,484,483}; AddFace(i, 4); }
{ int i[] = {343,342,357,358}; AddFace(i, 4); }
{ int i[] = {335,334,343,358}; AddFace(i, 4); }
{ int i[] = {345,220,359,360}; AddFace(i, 4); }
{ int i[] = {219,345,360,361}; AddFace(i, 4); }
{ int i[] = {196,219,361,362}; AddFace(i, 4); }
{ int i[] = {339,196,362,363}; AddFace(i, 4); }
{ int i[] = {235,339,363,364}; AddFace(i, 4); }
{ int i[] = {198,235,364,365}; AddFace(i, 4); }
{ int i[] = {200,198,365,366}; AddFace(i, 4); }
{ int i[] = {202,200,366,367}; AddFace(i, 4); }
{ int i[] = {323,202,367,368}; AddFace(i, 4); }
{ int i[] = {247,323,368,369}; AddFace(i, 4); }
{ int i[] = {240,247,369,370}; AddFace(i, 4); }
{ int i[] = {160,240,370,371}; AddFace(i, 4); }
{ int i[] = {162,160,371,372}; AddFace(i, 4); }
{ int i[] = {337,162,372,373}; AddFace(i, 4); }
{ int i[] = {164,337,373,374}; AddFace(i, 4); }
{ int i[] = {166,164,374,375}; AddFace(i, 4); }
{ int i[] = {376,168,487,486}; AddFace(i, 4); }
{ int i[] = {385,383,538,537}; AddFace(i, 4); }
{ int i[] = {167,169,287,376}; AddFace(i, 4); }
{ int i[] = {168,377,488,487}; AddFace(i, 4); }
{ int i[] = {164,166,378,379}; AddFace(i, 4); }
{ int i[] = {337,164,379,380}; AddFace(i, 4); }
{ int i[] = {162,337,380,381}; AddFace(i, 4); }
{ int i[] = {160,162,381,382}; AddFace(i, 4); }
{ int i[] = {240,160,341,237}; AddFace(i, 4); }
{ int i[] = {191,383,385,384}; AddFace(i, 4); }
{ int i[] = {189,191,384,386}; AddFace(i, 4); }
{ int i[] = {187,189,386,387}; AddFace(i, 4); }
{ int i[] = {231,187,387,388}; AddFace(i, 4); }
{ int i[] = {231,388,390,389}; AddFace(i, 4); }
{ int i[] = {384,385,537,536}; AddFace(i, 4); }
{ int i[] = {185,389,390,532}; AddFace(i, 4); }
{ int i[] = {358,357,391,392}; AddFace(i, 4); }
{ int i[] = {335,358,392,393}; AddFace(i, 4); }
{ int i[] = {207,335,393,394}; AddFace(i, 4); }
{ int i[] = {208,207,394,395}; AddFace(i, 4); }
{ int i[] = {396,209,491,490}; AddFace(i, 4); }
{ int i[] = {210,209,396,397}; AddFace(i, 4); }
{ int i[] = {268,210,397,398}; AddFace(i, 4); }
{ int i[] = {172,268,398,399}; AddFace(i, 4); }
{ int i[] = {286,172,399,400}; AddFace(i, 4); }
{ int i[] = {171,286,400,401}; AddFace(i, 4); }
{ int i[] = {341,204,391,356}; AddFace(i, 4); }
{ int i[] = {148,150,456,459}; AddFace(i, 4); }
{ int i[] = {296,295,457,458}; AddFace(i, 4); }
{ int i[] = {456,296,458,459}; AddFace(i, 4); }
{ int i[] = {225,148,459,330}; AddFace(i, 4); }
{ int i[] = {186,232,318,460}; AddFace(i, 4); }
{ int i[] = {316,317,319,461}; AddFace(i, 4); }
{ int i[] = {297,457,460,461}; AddFace(i, 4); }
{ int i[] = {260,297,461,319}; AddFace(i, 4); }
{ int i[] = {295,186,460,457}; AddFace(i, 4); }
{ int i[] = {168,211,212,377}; AddFace(i, 4); }
{ int i[] = {320,326,462,463}; AddFace(i, 4); }
{ int i[] = {261,320,463,464}; AddFace(i, 4); }
{ int i[] = {274,261,464,465}; AddFace(i, 4); }
{ int i[] = {275,274,465,466}; AddFace(i, 4); }
{ int i[] = {278,275,466,467}; AddFace(i, 4); }
{ int i[] = {307,278,467,468}; AddFace(i, 4); }
{ int i[] = {307,468,501,504}; AddFace(i, 4); }
{ int i[] = {469,264,505,500}; AddFace(i, 4); }
{ int i[] = {206,264,469,470}; AddFace(i, 4); }
{ int i[] = {205,206,470,471}; AddFace(i, 4); }
{ int i[] = {238,205,471,472}; AddFace(i, 4); }
{ int i[] = {249,238,472,473}; AddFace(i, 4); }
{ int i[] = {326,249,473,462}; AddFace(i, 4); }
{ int i[] = {179,158,281,282}; AddFace(i, 4); }
{ int i[] = {522,280,526,523}; AddFace(i, 4); }
{ int i[] = {312,310,474,475}; AddFace(i, 4); }
{ int i[] = {306,312,475,476}; AddFace(i, 4); }
{ int i[] = {306,476,478,477}; AddFace(i, 4); }
{ int i[] = {306,477,507,502}; AddFace(i, 4); }
{ int i[] = {479,180,503,506}; AddFace(i, 4); }
{ int i[] = {334,333,481,480}; AddFace(i, 4); }
{ int i[] = {343,334,480,482}; AddFace(i, 4); }
{ int i[] = {342,343,482,483}; AddFace(i, 4); }
{ int i[] = {357,342,483,484}; AddFace(i, 4); }
{ int i[] = {333,180,479,481}; AddFace(i, 4); }
{ int i[] = {278,307,477,478}; AddFace(i, 4); }
{ int i[] = {375,165,485,486}; AddFace(i, 4); }
{ int i[] = {166,375,486,487}; AddFace(i, 4); }
{ int i[] = {378,166,487,488}; AddFace(i, 4); }
{ int i[] = {213,378,488,489}; AddFace(i, 4); }
{ int i[] = {395,213,489,490}; AddFace(i, 4); }
{ int i[] = {208,395,490,491}; AddFace(i, 4); }
{ int i[] = {266,208,491,492}; AddFace(i, 4); }
{ int i[] = {176,266,492,493}; AddFace(i, 4); }
{ int i[] = {284,176,493,494}; AddFace(i, 4); }
{ int i[] = {175,284,494,495}; AddFace(i, 4); }
{ int i[] = {496,497,499,498}; AddFace(i, 4); }
{ int i[] = {497,496,502,503}; AddFace(i, 4); }
{ int i[] = {498,499,500,501}; AddFace(i, 4); }
{ int i[] = {501,500,505,504}; AddFace(i, 4); }
{ int i[] = {503,502,507,506}; AddFace(i, 4); }
{ int i[] = {504,505,506,507}; AddFace(i, 4); }
{ int i[] = {121,508,530,529}; AddFace(i, 4); }
{ int i[] = {125,124,510,511}; AddFace(i, 4); }
{ int i[] = {239,125,511,512}; AddFace(i, 4); }
{ int i[] = {250,239,512,520}; AddFace(i, 4); }
{ int i[] = {510,124,521,514}; AddFace(i, 4); }
{ int i[] = {509,123,515,514}; AddFace(i, 4); }
{ int i[] = {511,510,514,516}; AddFace(i, 4); }
{ int i[] = {512,511,516,518}; AddFace(i, 4); }
{ int i[] = {512,518,513,520}; AddFace(i, 4); }
{ int i[] = {127,126,513,251}; AddFace(i, 4); }
{ int i[] = {122,509,514,521}; AddFace(i, 4); }
{ int i[] = {522,121,529,528}; AddFace(i, 4); }
{ int i[] = {508,121,522,523}; AddFace(i, 4); }
{ int i[] = {120,508,523,524}; AddFace(i, 4); }
{ int i[] = {524,523,526,525}; AddFace(i, 4); }
{ int i[] = {281,158,525,526}; AddFace(i, 4); }
{ int i[] = {280,157,281,526}; AddFace(i, 4); }
{ int i[] = {521,124,527,528}; AddFace(i, 4); }
{ int i[] = {122,521,528,529}; AddFace(i, 4); }
{ int i[] = {509,122,529,530}; AddFace(i, 4); }
{ int i[] = {123,509,530,531}; AddFace(i, 4); }
{ int i[] = {263,157,280,527}; AddFace(i, 4); }
{ int i[] = {184,289,288,295}; AddFace(i, 4); }
{ int i[] = {186,295,288,532}; AddFace(i, 4); }
{ int i[] = {188,232,533,534}; AddFace(i, 4); }
{ int i[] = {190,188,534,535}; AddFace(i, 4); }
{ int i[] = {192,190,535,536}; AddFace(i, 4); }
{ int i[] = {359,192,536,537}; AddFace(i, 4); }
{ int i[] = {193,359,537,538}; AddFace(i, 4); }
{ int i[] = {390,388,533,532}; AddFace(i, 4); }
{ int i[] = {317,223,325,539}; AddFace(i, 4); }
{ int i[] = {319,317,539,540}; AddFace(i, 4); }
{ int i[] = {326,320,540,539}; AddFace(i, 4); }
{ int i[] = {273,260,541,542}; AddFace(i, 4); }
{ int i[] = {244,273,542,543}; AddFace(i, 4); }
{ int i[] = {299,244,543,544}; AddFace(i, 4); }
{ int i[] = {245,299,544,545}; AddFace(i, 4); }
{ int i[] = {260,319,540,541}; AddFace(i, 4); }
{ int i[] = {128,130,132,134}; AddFace(i, 4); }
{ int i[] = {134,136,138,140}; AddFace(i, 4); }
{ int i[] = {140,142,144,146}; AddFace(i, 4); }
{ int i[] = {134,222,329,128}; AddFace(i, 4); }
{ int i[] = {129,128,329,546}; AddFace(i, 4); }
{ int i[] = {430,431,428,429}; AddFace(i, 4); }
{ int i[] = {427,426,547,548}; AddFace(i, 4); }
{ int i[] = {445,447,548,547}; AddFace(i, 4); }
{ int i[] = {428,427,548,549}; AddFace(i, 4); }
{ int i[] = {447,449,549,548}; AddFace(i, 4); }
{ int i[] = {428,549,550,429}; AddFace(i, 4); }
{ int i[] = {449,451,550,549}; AddFace(i, 4); }
{ int i[] = {429,550,551,430}; AddFace(i, 4); }
{ int i[] = {451,453,551,550}; AddFace(i, 4); }
{ int i[] = {430,551,552,431}; AddFace(i, 4); }
{ int i[] = {453,455,552,551}; AddFace(i, 4); }
{ int i[] = {426,431,552,547}; AddFace(i, 4); }
{ int i[] = {455,445,547,552}; AddFace(i, 4); }
{ int i[] = {433,446,444,432}; AddFace(i, 4); }
{ int i[] = {434,448,446,433}; AddFace(i, 4); }
{ int i[] = {435,450,448,434}; AddFace(i, 4); }
{ int i[] = {436,452,450,435}; AddFace(i, 4); }
{ int i[] = {437,454,452,436}; AddFace(i, 4); }
{ int i[] = {444,454,437,432}; AddFace(i, 4); }
{ int i[] = {432,553,554,433}; AddFace(i, 4); }
{ int i[] = {438,439,554,553}; AddFace(i, 4); }
{ int i[] = {433,554,555,434}; AddFace(i, 4); }
{ int i[] = {439,440,555,554}; AddFace(i, 4); }
{ int i[] = {434,555,556,435}; AddFace(i, 4); }
{ int i[] = {440,441,556,555}; AddFace(i, 4); }
{ int i[] = {435,556,557,436}; AddFace(i, 4); }
{ int i[] = {441,442,557,556}; AddFace(i, 4); }
{ int i[] = {436,557,558,437}; AddFace(i, 4); }
{ int i[] = {442,443,558,557}; AddFace(i, 4); }
{ int i[] = {437,558,553,432}; AddFace(i, 4); }
{ int i[] = {443,438,553,558}; AddFace(i, 4); }
{ int i[] = {443,440,439,438}; AddFace(i, 4); }
{ int i[] = {428,431,426,427}; AddFace(i, 4); }
{ int i[] = {442,441,440,443}; AddFace(i, 4); }
{ int i[] = {455,454,444,445}; AddFace(i, 4); }
{ int i[] = {446,447,445,444}; AddFace(i, 4); }
{ int i[] = {448,449,447,446}; AddFace(i, 4); }
{ int i[] = {450,451,449,448}; AddFace(i, 4); }
{ int i[] = {452,453,451,450}; AddFace(i, 4); }
{ int i[] = {455,453,452,454}; AddFace(i, 4); }
{ int i[] = {406,407,404,405}; AddFace(i, 4); }
{ int i[] = {403,402,559,560}; AddFace(i, 4); }
{ int i[] = {559,571,572,560}; AddFace(i, 4); }
{ int i[] = {404,403,560,561}; AddFace(i, 4); }
{ int i[] = {560,572,573,561}; AddFace(i, 4); }
{ int i[] = {404,561,562,405}; AddFace(i, 4); }
{ int i[] = {561,573,574,562}; AddFace(i, 4); }
{ int i[] = {405,562,563,406}; AddFace(i, 4); }
{ int i[] = {562,574,575,563}; AddFace(i, 4); }
{ int i[] = {406,563,564,407}; AddFace(i, 4); }
{ int i[] = {563,575,576,564}; AddFace(i, 4); }
{ int i[] = {402,407,564,559}; AddFace(i, 4); }
{ int i[] = {564,576,571,559}; AddFace(i, 4); }
{ int i[] = {409,421,420,408}; AddFace(i, 4); }
{ int i[] = {410,422,421,409}; AddFace(i, 4); }
{ int i[] = {411,423,422,410}; AddFace(i, 4); }
{ int i[] = {412,424,423,411}; AddFace(i, 4); }
{ int i[] = {413,425,424,412}; AddFace(i, 4); }
{ int i[] = {420,425,413,408}; AddFace(i, 4); }
{ int i[] = {408,565,566,409}; AddFace(i, 4); }
{ int i[] = {414,415,566,565}; AddFace(i, 4); }
{ int i[] = {409,566,567,410}; AddFace(i, 4); }
{ int i[] = {415,416,567,566}; AddFace(i, 4); }
{ int i[] = {410,567,568,411}; AddFace(i, 4); }
{ int i[] = {416,417,568,567}; AddFace(i, 4); }
{ int i[] = {411,568,569,412}; AddFace(i, 4); }
{ int i[] = {417,418,569,568}; AddFace(i, 4); }
{ int i[] = {412,569,570,413}; AddFace(i, 4); }
{ int i[] = {418,419,570,569}; AddFace(i, 4); }
{ int i[] = {413,570,565,408}; AddFace(i, 4); }
{ int i[] = {419,414,565,570}; AddFace(i, 4); }
{ int i[] = {419,416,415,414}; AddFace(i, 4); }
{ int i[] = {404,407,402,403}; AddFace(i, 4); }
{ int i[] = {418,417,416,419}; AddFace(i, 4); }
{ int i[] = {571,576,425,420}; AddFace(i, 4); }
{ int i[] = {421,572,571,420}; AddFace(i, 4); }
{ int i[] = {422,573,572,421}; AddFace(i, 4); }
{ int i[] = {423,574,573,422}; AddFace(i, 4); }
{ int i[] = {424,575,574,423}; AddFace(i, 4); }
{ int i[] = {425,576,575,424}; AddFace(i, 4); }
{ int i[] = {580,619,620,578}; AddFace(i, 4); }
{ int i[] = {599,614,615,598}; AddFace(i, 4); }
{ int i[] = {584,607,608,582}; AddFace(i, 4); }
{ int i[] = {578,620,607,584}; AddFace(i, 4); }
{ int i[] = {578,584,582,580}; AddFace(i, 4); }
{ int i[] = {579,581,583,577}; AddFace(i, 4); }
{ int i[] = {586,618,619,580}; AddFace(i, 4); }
{ int i[] = {582,587,586,580}; AddFace(i, 4); }
{ int i[] = {582,608,609,587}; AddFace(i, 4); }
{ int i[] = {579,585,588,581}; AddFace(i, 4); }
{ int i[] = {617,621,622,590}; AddFace(i, 4); }
{ int i[] = {590,622,623,591}; AddFace(i, 4); }
{ int i[] = {591,623,624,610}; AddFace(i, 4); }
{ int i[] = {592,625,626,589}; AddFace(i, 4); }
{ int i[] = {594,616,617,590}; AddFace(i, 4); }
{ int i[] = {591,595,594,590}; AddFace(i, 4); }
{ int i[] = {602,611,612,604}; AddFace(i, 4); }
{ int i[] = {589,593,596,592}; AddFace(i, 4); }
{ int i[] = {598,615,616,594}; AddFace(i, 4); }
{ int i[] = {595,599,598,594}; AddFace(i, 4); }
{ int i[] = {604,612,613,606}; AddFace(i, 4); }
{ int i[] = {593,597,600,596}; AddFace(i, 4); }
{ int i[] = {591,610,611,602}; AddFace(i, 4); }
{ int i[] = {596,603,601,592}; AddFace(i, 4); }
{ int i[] = {591,602,604,595}; AddFace(i, 4); }
{ int i[] = {600,605,603,596}; AddFace(i, 4); }
{ int i[] = {606,613,614,599}; AddFace(i, 4); }
{ int i[] = {595,604,606,599}; AddFace(i, 4); }
{ int i[] = {581,608,607,583}; AddFace(i, 4); }
{ int i[] = {588,609,608,581}; AddFace(i, 4); }
{ int i[] = {610,624,625,592}; AddFace(i, 4); }
{ int i[] = {601,611,610,592}; AddFace(i, 4); }
{ int i[] = {603,612,611,601}; AddFace(i, 4); }
{ int i[] = {605,613,612,603}; AddFace(i, 4); }
{ int i[] = {600,614,613,605}; AddFace(i, 4); }
{ int i[] = {597,615,614,600}; AddFace(i, 4); }
{ int i[] = {593,616,615,597}; AddFace(i, 4); }
{ int i[] = {589,617,616,593}; AddFace(i, 4); }
{ int i[] = {589,626,621,617}; AddFace(i, 4); }
{ int i[] = {579,619,618,585}; AddFace(i, 4); }
{ int i[] = {577,620,619,579}; AddFace(i, 4); }
{ int i[] = {583,607,620,577}; AddFace(i, 4); }
{ int i[] = {621,632,627,622}; AddFace(i, 4); }
{ int i[] = {622,627,628,623}; AddFace(i, 4); }
{ int i[] = {623,628,629,624}; AddFace(i, 4); }
{ int i[] = {624,629,630,625}; AddFace(i, 4); }
{ int i[] = {625,630,631,626}; AddFace(i, 4); }
{ int i[] = {626,631,632,621}; AddFace(i, 4); }
{ int i[] = {587,628,627,586}; AddFace(i, 4); }
{ int i[] = {609,629,628,587}; AddFace(i, 4); }
{ int i[] = {588,630,629,609}; AddFace(i, 4); }
{ int i[] = {585,631,630,588}; AddFace(i, 4); }
{ int i[] = {618,632,631,585}; AddFace(i, 4); }
{ int i[] = {586,627,632,618}; AddFace(i, 4); }
{ int i[] = {954,971,531,120}; AddFace(i, 4); }
{ int i[] = {745,968,969,965}; AddFace(i, 4); }
{ int i[] = {960,961,517,515}; AddFace(i, 4); }
{ int i[] = {635,722,721,636}; AddFace(i, 4); }
{ int i[] = {961,962,519,517}; AddFace(i, 4); }
{ int i[] = {636,721,720,709}; AddFace(i, 4); }
{ int i[] = {637,785,546,127}; AddFace(i, 4); }
{ int i[] = {718,784,785,637}; AddFace(i, 4); }
{ int i[] = {637,959,963,716}; AddFace(i, 4); }
{ int i[] = {638,639,131,129}; AddFace(i, 4); }
{ int i[] = {639,640,133,131}; AddFace(i, 4); }
{ int i[] = {640,641,135,133}; AddFace(i, 4); }
{ int i[] = {641,642,137,135}; AddFace(i, 4); }
{ int i[] = {694,697,644,641}; AddFace(i, 4); }
{ int i[] = {642,643,139,137}; AddFace(i, 4); }
{ int i[] = {643,644,141,139}; AddFace(i, 4); }
{ int i[] = {644,645,143,141}; AddFace(i, 4); }
{ int i[] = {697,648,647,644}; AddFace(i, 4); }
{ int i[] = {645,646,145,143}; AddFace(i, 4); }
{ int i[] = {646,647,147,145}; AddFace(i, 4); }
{ int i[] = {647,648,149,147}; AddFace(i, 4); }
{ int i[] = {648,649,151,149}; AddFace(i, 4); }
{ int i[] = {649,905,757,650}; AddFace(i, 4); }
{ int i[] = {649,650,294,151}; AddFace(i, 4); }
{ int i[] = {154,764,759,242}; AddFace(i, 4); }
{ int i[] = {650,757,651,755}; AddFace(i, 4); }
{ int i[] = {293,763,764,154}; AddFace(i, 4); }
{ int i[] = {652,665,181,156}; AddFace(i, 4); }
{ int i[] = {652,741,744,665}; AddFace(i, 4); }
{ int i[] = {156,242,759,652}; AddFace(i, 4); }
{ int i[] = {653,943,945,728}; AddFace(i, 4); }
{ int i[] = {653,746,747,664}; AddFace(i, 4); }
{ int i[] = {241,822,823,159}; AddFace(i, 4); }
{ int i[] = {322,820,821,246}; AddFace(i, 4); }
{ int i[] = {159,823,824,161}; AddFace(i, 4); }
{ int i[] = {675,833,834,676}; AddFace(i, 4); }
{ int i[] = {794,676,834,654}; AddFace(i, 4); }
{ int i[] = {161,824,825,338}; AddFace(i, 4); }
{ int i[] = {791,832,833,675}; AddFace(i, 4); }
{ int i[] = {163,826,827,165}; AddFace(i, 4); }
{ int i[] = {685,830,831,686}; AddFace(i, 4); }
{ int i[] = {167,485,933,828}; AddFace(i, 4); }
{ int i[] = {829,935,936,684}; AddFace(i, 4); }
{ int i[] = {659,752,828,658}; AddFace(i, 4); }
{ int i[] = {658,683,734,659}; AddFace(i, 4); }
{ int i[] = {752,850,401,169}; AddFace(i, 4); }
{ int i[] = {734,848,849,659}; AddFace(i, 4); }
{ int i[] = {171,751,750,173}; AddFace(i, 4); }
{ int i[] = {660,733,732,661}; AddFace(i, 4); }
{ int i[] = {659,849,850,752}; AddFace(i, 4); }
{ int i[] = {750,941,495,173}; AddFace(i, 4); }
{ int i[] = {732,939,940,661}; AddFace(i, 4); }
{ int i[] = {661,750,751,660}; AddFace(i, 4); }
{ int i[] = {175,749,748,177}; AddFace(i, 4); }
{ int i[] = {662,731,730,663}; AddFace(i, 4); }
{ int i[] = {661,940,941,750}; AddFace(i, 4); }
{ int i[] = {179,331,787,747}; AddFace(i, 4); }
{ int i[] = {927,929,928,729}; AddFace(i, 4); }
{ int i[] = {663,748,749,662}; AddFace(i, 4); }
{ int i[] = {664,949,943,653}; AddFace(i, 4); }
{ int i[] = {747,787,788,664}; AddFace(i, 4); }
{ int i[] = {665,983,545,181}; AddFace(i, 4); }
{ int i[] = {665,744,743,740}; AddFace(i, 4); }
{ int i[] = {245,760,761,183}; AddFace(i, 4); }
{ int i[] = {666,712,711,651}; AddFace(i, 4); }
{ int i[] = {183,761,762,291}; AddFace(i, 4); }
{ int i[] = {290,753,972,185}; AddFace(i, 4); }
{ int i[] = {703,973,972,667}; AddFace(i, 4); }
{ int i[] = {758,906,907,727}; AddFace(i, 4); }
{ int i[] = {839,973,974,838}; AddFace(i, 4); }
{ int i[] = {838,974,975,837}; AddFace(i, 4); }
{ int i[] = {668,688,689,669}; AddFace(i, 4); }
{ int i[] = {668,703,704,688}; AddFace(i, 4); }
{ int i[] = {837,975,976,835}; AddFace(i, 4); }
{ int i[] = {669,689,693,670}; AddFace(i, 4); }
{ int i[] = {693,692,811,670}; AddFace(i, 4); }
{ int i[] = {344,812,813,194}; AddFace(i, 4); }
{ int i[] = {194,813,814,195}; AddFace(i, 4); }
{ int i[] = {195,814,815,340}; AddFace(i, 4); }
{ int i[] = {236,816,817,197}; AddFace(i, 4); }
{ int i[] = {197,817,818,199}; AddFace(i, 4); }
{ int i[] = {672,699,698,673}; AddFace(i, 4); }
{ int i[] = {672,706,705,699}; AddFace(i, 4); }
{ int i[] = {199,818,819,201}; AddFace(i, 4); }
{ int i[] = {673,698,687,674}; AddFace(i, 4); }
{ int i[] = {713,779,780,714}; AddFace(i, 4); }
{ int i[] = {676,841,842,675}; AddFace(i, 4); }
{ int i[] = {808,932,809,841}; AddFace(i, 4); }
{ int i[] = {728,918,919,722}; AddFace(i, 4); }
{ int i[] = {679,730,731,680}; AddFace(i, 4); }
{ int i[] = {686,844,845,685}; AddFace(i, 4); }
{ int i[] = {681,938,939,732}; AddFace(i, 4); }
{ int i[] = {684,936,937,846}; AddFace(i, 4); }
{ int i[] = {681,732,733,682}; AddFace(i, 4); }
{ int i[] = {684,846,847,683}; AddFace(i, 4); }
{ int i[] = {683,847,848,734}; AddFace(i, 4); }
{ int i[] = {780,805,806,714}; AddFace(i, 4); }
{ int i[] = {698,803,804,687}; AddFace(i, 4); }
{ int i[] = {701,799,800,700}; AddFace(i, 4); }
{ int i[] = {700,800,801,705}; AddFace(i, 4); }
{ int i[] = {702,798,799,701}; AddFace(i, 4); }
{ int i[] = {701,700,793,671}; AddFace(i, 4); }
{ int i[] = {691,702,701,671}; AddFace(i, 4); }
{ int i[] = {691,797,798,702}; AddFace(i, 4); }
{ int i[] = {694,717,786,697}; AddFace(i, 4); }
{ int i[] = {699,802,803,698}; AddFace(i, 4); }
{ int i[] = {705,801,802,699}; AddFace(i, 4); }
{ int i[] = {714,806,807,707}; AddFace(i, 4); }
{ int i[] = {721,920,921,720}; AddFace(i, 4); }
{ int i[] = {251,519,962,959}; AddFace(i, 4); }
{ int i[] = {709,720,719,716}; AddFace(i, 4); }
{ int i[] = {710,713,714,707}; AddFace(i, 4); }
{ int i[] = {246,821,822,241}; AddFace(i, 4); }
{ int i[] = {711,737,736,735}; AddFace(i, 4); }
{ int i[] = {712,738,737,711}; AddFace(i, 4); }
{ int i[] = {739,981,982,740}; AddFace(i, 4); }
{ int i[] = {720,921,922,719}; AddFace(i, 4); }
{ int i[] = {719,922,911,783}; AddFace(i, 4); }
{ int i[] = {716,719,718,637}; AddFace(i, 4); }
{ int i[] = {723,736,737,724}; AddFace(i, 4); }
{ int i[] = {724,737,738,725}; AddFace(i, 4); }
{ int i[] = {725,758,727,724}; AddFace(i, 4); }
{ int i[] = {726,980,981,739}; AddFace(i, 4); }
{ int i[] = {723,913,914,736}; AddFace(i, 4); }
{ int i[] = {728,945,946,918}; AddFace(i, 4); }
{ int i[] = {729,951,952,927}; AddFace(i, 4); }
{ int i[] = {735,742,741,652}; AddFace(i, 4); }
{ int i[] = {736,914,915,735}; AddFace(i, 4); }
{ int i[] = {735,915,916,742}; AddFace(i, 4); }
{ int i[] = {742,766,765,741}; AddFace(i, 4); }
{ int i[] = {742,916,917,766}; AddFace(i, 4); }
{ int i[] = {744,924,926,743}; AddFace(i, 4); }
{ int i[] = {650,755,754,753}; AddFace(i, 4); }
{ int i[] = {291,762,763,293}; AddFace(i, 4); }
{ int i[] = {753,290,294,650}; AddFace(i, 4); }
{ int i[] = {651,757,756,666}; AddFace(i, 4); }
{ int i[] = {719,783,784,718}; AddFace(i, 4); }
{ int i[] = {775,909,910,773}; AddFace(i, 4); }
{ int i[] = {652,759,711,735}; AddFace(i, 4); }
{ int i[] = {666,761,760,712}; AddFace(i, 4); }
{ int i[] = {754,762,761,666}; AddFace(i, 4); }
{ int i[] = {755,763,762,754}; AddFace(i, 4); }
{ int i[] = {651,764,763,755}; AddFace(i, 4); }
{ int i[] = {711,759,764,651}; AddFace(i, 4); }
{ int i[] = {766,944,942,765}; AddFace(i, 4); }
{ int i[] = {765,942,948,767}; AddFace(i, 4); }
{ int i[] = {917,947,944,766}; AddFace(i, 4); }
{ int i[] = {925,953,950,768}; AddFace(i, 4); }
{ int i[] = {308,771,772,309}; AddFace(i, 4); }
{ int i[] = {309,772,769,310}; AddFace(i, 4); }
{ int i[] = {311,474,923,770}; AddFace(i, 4); }
{ int i[] = {311,770,771,308}; AddFace(i, 4); }
{ int i[] = {770,923,924,744}; AddFace(i, 4); }
{ int i[] = {741,771,770,744}; AddFace(i, 4); }
{ int i[] = {765,772,771,741}; AddFace(i, 4); }
{ int i[] = {767,769,772,765}; AddFace(i, 4); }
{ int i[] = {740,982,983,665}; AddFace(i, 4); }
{ int i[] = {696,773,774,695}; AddFace(i, 4); }
{ int i[] = {690,775,773,696}; AddFace(i, 4); }
{ int i[] = {704,703,775,690}; AddFace(i, 4); }
{ int i[] = {777,979,980,726}; AddFace(i, 4); }
{ int i[] = {778,912,913,723}; AddFace(i, 4); }
{ int i[] = {201,819,820,322}; AddFace(i, 4); }
{ int i[] = {687,780,779,674}; AddFace(i, 4); }
{ int i[] = {687,804,805,780}; AddFace(i, 4); }
{ int i[] = {781,978,782,715}; AddFace(i, 4); }
{ int i[] = {783,911,912,778}; AddFace(i, 4); }
{ int i[] = {717,784,783,778}; AddFace(i, 4); }
{ int i[] = {694,785,784,717}; AddFace(i, 4); }
{ int i[] = {717,778,723,724}; AddFace(i, 4); }
{ int i[] = {724,727,786,717}; AddFace(i, 4); }
{ int i[] = {727,907,908,786}; AddFace(i, 4); }
{ int i[] = {635,968,728,722}; AddFace(i, 4); }
{ int i[] = {722,919,920,721}; AddFace(i, 4); }
{ int i[] = {675,842,843,791}; AddFace(i, 4); }
{ int i[] = {748,787,331,177}; AddFace(i, 4); }
{ int i[] = {663,788,787,748}; AddFace(i, 4); }
{ int i[] = {730,789,788,663}; AddFace(i, 4); }
{ int i[] = {679,790,789,730}; AddFace(i, 4); }
{ int i[] = {791,843,844,686}; AddFace(i, 4); }
{ int i[] = {686,831,832,791}; AddFace(i, 4); }
{ int i[] = {338,825,826,163}; AddFace(i, 4); }
{ int i[] = {340,815,816,236}; AddFace(i, 4); }
{ int i[] = {705,706,793,700}; AddFace(i, 4); }
{ int i[] = {707,807,808,794}; AddFace(i, 4); }
{ int i[] = {678,930,931,677}; AddFace(i, 4); }
{ int i[] = {729,928,930,678}; AddFace(i, 4); }
{ int i[] = {193,811,812,344}; AddFace(i, 4); }
{ int i[] = {693,798,797,692}; AddFace(i, 4); }
{ int i[] = {689,799,798,693}; AddFace(i, 4); }
{ int i[] = {688,800,799,689}; AddFace(i, 4); }
{ int i[] = {704,801,800,688}; AddFace(i, 4); }
{ int i[] = {690,802,801,704}; AddFace(i, 4); }
{ int i[] = {696,803,802,690}; AddFace(i, 4); }
{ int i[] = {695,804,803,696}; AddFace(i, 4); }
{ int i[] = {781,805,804,695}; AddFace(i, 4); }
{ int i[] = {715,806,805,781}; AddFace(i, 4); }
{ int i[] = {708,807,806,715}; AddFace(i, 4); }
{ int i[] = {677,808,807,708}; AddFace(i, 4); }
{ int i[] = {677,931,932,808}; AddFace(i, 4); }
{ int i[] = {796,810,809,795}; AddFace(i, 4); }
{ int i[] = {790,810,796,789}; AddFace(i, 4); }
{ int i[] = {797,812,811,692}; AddFace(i, 4); }
{ int i[] = {691,813,812,797}; AddFace(i, 4); }
{ int i[] = {671,814,813,691}; AddFace(i, 4); }
{ int i[] = {793,815,814,671}; AddFace(i, 4); }
{ int i[] = {706,816,815,793}; AddFace(i, 4); }
{ int i[] = {672,817,816,706}; AddFace(i, 4); }
{ int i[] = {673,818,817,672}; AddFace(i, 4); }
{ int i[] = {674,819,818,673}; AddFace(i, 4); }
{ int i[] = {779,820,819,674}; AddFace(i, 4); }
{ int i[] = {713,821,820,779}; AddFace(i, 4); }
{ int i[] = {710,822,821,713}; AddFace(i, 4); }
{ int i[] = {654,823,822,710}; AddFace(i, 4); }
{ int i[] = {655,824,823,654}; AddFace(i, 4); }
{ int i[] = {792,825,824,655}; AddFace(i, 4); }
{ int i[] = {656,826,825,792}; AddFace(i, 4); }
{ int i[] = {657,827,826,656}; AddFace(i, 4); }
{ int i[] = {828,933,934,658}; AddFace(i, 4); }
{ int i[] = {836,977,538,383}; AddFace(i, 4); }
{ int i[] = {167,828,752,169}; AddFace(i, 4); }
{ int i[] = {658,934,935,829}; AddFace(i, 4); }
{ int i[] = {656,831,830,657}; AddFace(i, 4); }
{ int i[] = {792,832,831,656}; AddFace(i, 4); }
{ int i[] = {655,833,832,792}; AddFace(i, 4); }
{ int i[] = {654,834,833,655}; AddFace(i, 4); }
{ int i[] = {710,707,794,654}; AddFace(i, 4); }
{ int i[] = {191,835,836,383}; AddFace(i, 4); }
{ int i[] = {189,837,835,191}; AddFace(i, 4); }
{ int i[] = {187,838,837,189}; AddFace(i, 4); }
{ int i[] = {231,839,838,187}; AddFace(i, 4); }
{ int i[] = {231,389,840,839}; AddFace(i, 4); }
{ int i[] = {835,976,977,836}; AddFace(i, 4); }
{ int i[] = {185,972,840,389}; AddFace(i, 4); }
{ int i[] = {810,842,841,809}; AddFace(i, 4); }
{ int i[] = {790,843,842,810}; AddFace(i, 4); }
{ int i[] = {679,844,843,790}; AddFace(i, 4); }
{ int i[] = {680,845,844,679}; AddFace(i, 4); }
{ int i[] = {846,937,938,681}; AddFace(i, 4); }
{ int i[] = {682,847,846,681}; AddFace(i, 4); }
{ int i[] = {733,848,847,682}; AddFace(i, 4); }
{ int i[] = {660,849,848,733}; AddFace(i, 4); }
{ int i[] = {751,850,849,660}; AddFace(i, 4); }
{ int i[] = {171,401,850,751}; AddFace(i, 4); }
{ int i[] = {794,808,841,676}; AddFace(i, 4); }
{ int i[] = {648,908,905,649}; AddFace(i, 4); }
{ int i[] = {757,907,906,756}; AddFace(i, 4); }
{ int i[] = {905,908,907,757}; AddFace(i, 4); }
{ int i[] = {697,786,908,648}; AddFace(i, 4); }
{ int i[] = {667,909,775,703}; AddFace(i, 4); }
{ int i[] = {773,910,776,774}; AddFace(i, 4); }
{ int i[] = {758,910,909,906}; AddFace(i, 4); }
{ int i[] = {725,776,910,758}; AddFace(i, 4); }
{ int i[] = {756,906,909,667}; AddFace(i, 4); }
{ int i[] = {658,829,684,683}; AddFace(i, 4); }
{ int i[] = {777,912,911,782}; AddFace(i, 4); }
{ int i[] = {726,913,912,777}; AddFace(i, 4); }
{ int i[] = {739,914,913,726}; AddFace(i, 4); }
{ int i[] = {740,915,914,739}; AddFace(i, 4); }
{ int i[] = {743,916,915,740}; AddFace(i, 4); }
{ int i[] = {768,917,916,743}; AddFace(i, 4); }
{ int i[] = {768,950,947,917}; AddFace(i, 4); }
{ int i[] = {918,946,951,729}; AddFace(i, 4); }
{ int i[] = {678,919,918,729}; AddFace(i, 4); }
{ int i[] = {677,920,919,678}; AddFace(i, 4); }
{ int i[] = {708,921,920,677}; AddFace(i, 4); }
{ int i[] = {715,922,921,708}; AddFace(i, 4); }
{ int i[] = {782,911,922,715}; AddFace(i, 4); }
{ int i[] = {179,747,746,158}; AddFace(i, 4); }
{ int i[] = {965,966,967,745}; AddFace(i, 4); }
{ int i[] = {769,923,474,310}; AddFace(i, 4); }
{ int i[] = {767,924,923,769}; AddFace(i, 4); }
{ int i[] = {767,925,926,924}; AddFace(i, 4); }
{ int i[] = {767,948,953,925}; AddFace(i, 4); }
{ int i[] = {927,952,949,664}; AddFace(i, 4); }
{ int i[] = {789,928,929,788}; AddFace(i, 4); }
{ int i[] = {796,930,928,789}; AddFace(i, 4); }
{ int i[] = {795,931,930,796}; AddFace(i, 4); }
{ int i[] = {809,932,931,795}; AddFace(i, 4); }
{ int i[] = {788,929,927,664}; AddFace(i, 4); }
{ int i[] = {743,926,925,768}; AddFace(i, 4); }
{ int i[] = {827,933,485,165}; AddFace(i, 4); }
{ int i[] = {657,934,933,827}; AddFace(i, 4); }
{ int i[] = {830,935,934,657}; AddFace(i, 4); }
{ int i[] = {685,936,935,830}; AddFace(i, 4); }
{ int i[] = {845,937,936,685}; AddFace(i, 4); }
{ int i[] = {680,938,937,845}; AddFace(i, 4); }
{ int i[] = {731,939,938,680}; AddFace(i, 4); }
{ int i[] = {662,940,939,731}; AddFace(i, 4); }
{ int i[] = {749,941,940,662}; AddFace(i, 4); }
{ int i[] = {175,495,941,749}; AddFace(i, 4); }
{ int i[] = {942,944,945,943}; AddFace(i, 4); }
{ int i[] = {943,949,948,942}; AddFace(i, 4); }
{ int i[] = {944,947,946,945}; AddFace(i, 4); }
{ int i[] = {947,950,951,946}; AddFace(i, 4); }
{ int i[] = {949,952,953,948}; AddFace(i, 4); }
{ int i[] = {950,953,952,951}; AddFace(i, 4); }
{ int i[] = {633,970,971,954}; AddFace(i, 4); }
{ int i[] = {636,957,956,635}; AddFace(i, 4); }
{ int i[] = {709,958,957,636}; AddFace(i, 4); }
{ int i[] = {716,963,958,709}; AddFace(i, 4); }
{ int i[] = {956,960,964,635}; AddFace(i, 4); }
{ int i[] = {955,960,515,123}; AddFace(i, 4); }
{ int i[] = {957,961,960,956}; AddFace(i, 4); }
{ int i[] = {958,962,961,957}; AddFace(i, 4); }
{ int i[] = {958,963,959,962}; AddFace(i, 4); }
{ int i[] = {127,251,959,637}; AddFace(i, 4); }
{ int i[] = {634,964,960,955}; AddFace(i, 4); }
{ int i[] = {965,969,970,633}; AddFace(i, 4); }
{ int i[] = {954,966,965,633}; AddFace(i, 4); }
{ int i[] = {120,524,966,954}; AddFace(i, 4); }
{ int i[] = {524,525,967,966}; AddFace(i, 4); }
{ int i[] = {746,967,525,158}; AddFace(i, 4); }
{ int i[] = {745,967,746,653}; AddFace(i, 4); }
{ int i[] = {964,969,968,635}; AddFace(i, 4); }
{ int i[] = {634,970,969,964}; AddFace(i, 4); }
{ int i[] = {955,971,970,634}; AddFace(i, 4); }
{ int i[] = {123,531,971,955}; AddFace(i, 4); }
{ int i[] = {728,968,745,653}; AddFace(i, 4); }
{ int i[] = {666,756,753,754}; AddFace(i, 4); }
{ int i[] = {667,972,753,756}; AddFace(i, 4); }
{ int i[] = {668,974,973,703}; AddFace(i, 4); }
{ int i[] = {669,975,974,668}; AddFace(i, 4); }
{ int i[] = {670,976,975,669}; AddFace(i, 4); }
{ int i[] = {811,977,976,670}; AddFace(i, 4); }
{ int i[] = {193,538,977,811}; AddFace(i, 4); }
{ int i[] = {840,972,973,839}; AddFace(i, 4); }
{ int i[] = {774,978,781,695}; AddFace(i, 4); }
{ int i[] = {776,979,978,774}; AddFace(i, 4); }
{ int i[] = {782,978,979,777}; AddFace(i, 4); }
{ int i[] = {738,981,980,725}; AddFace(i, 4); }
{ int i[] = {712,982,981,738}; AddFace(i, 4); }
{ int i[] = {760,983,982,712}; AddFace(i, 4); }
{ int i[] = {245,545,983,760}; AddFace(i, 4); }
{ int i[] = {725,980,979,776}; AddFace(i, 4); }
{ int i[] = {638,641,640,639}; AddFace(i, 4); }
{ int i[] = {641,644,643,642}; AddFace(i, 4); }
{ int i[] = {644,647,646,645}; AddFace(i, 4); }
{ int i[] = {641,638,785,694}; AddFace(i, 4); }
{ int i[] = {129,546,785,638}; AddFace(i, 4); }
{ int i[] = {879,878,877,880}; AddFace(i, 4); }
{ int i[] = {876,985,984,875}; AddFace(i, 4); }
{ int i[] = {894,984,985,896}; AddFace(i, 4); }
{ int i[] = {877,986,985,876}; AddFace(i, 4); }
{ int i[] = {896,985,986,898}; AddFace(i, 4); }
{ int i[] = {877,878,987,986}; AddFace(i, 4); }
{ int i[] = {898,986,987,900}; AddFace(i, 4); }
{ int i[] = {878,879,988,987}; AddFace(i, 4); }
{ int i[] = {900,987,988,902}; AddFace(i, 4); }
{ int i[] = {879,880,989,988}; AddFace(i, 4); }
{ int i[] = {902,988,989,904}; AddFace(i, 4); }
{ int i[] = {875,984,989,880}; AddFace(i, 4); }
{ int i[] = {904,989,984,894}; AddFace(i, 4); }
{ int i[] = {882,881,893,895}; AddFace(i, 4); }
{ int i[] = {883,882,895,897}; AddFace(i, 4); }
{ int i[] = {884,883,897,899}; AddFace(i, 4); }
{ int i[] = {885,884,899,901}; AddFace(i, 4); }
{ int i[] = {886,885,901,903}; AddFace(i, 4); }
{ int i[] = {893,881,886,903}; AddFace(i, 4); }
{ int i[] = {881,882,991,990}; AddFace(i, 4); }
{ int i[] = {887,990,991,888}; AddFace(i, 4); }
{ int i[] = {882,883,992,991}; AddFace(i, 4); }
{ int i[] = {888,991,992,889}; AddFace(i, 4); }
{ int i[] = {883,884,993,992}; AddFace(i, 4); }
{ int i[] = {889,992,993,890}; AddFace(i, 4); }
{ int i[] = {884,885,994,993}; AddFace(i, 4); }
{ int i[] = {890,993,994,891}; AddFace(i, 4); }
{ int i[] = {885,886,995,994}; AddFace(i, 4); }
{ int i[] = {891,994,995,892}; AddFace(i, 4); }
{ int i[] = {886,881,990,995}; AddFace(i, 4); }
{ int i[] = {892,995,990,887}; AddFace(i, 4); }
{ int i[] = {892,887,888,889}; AddFace(i, 4); }
{ int i[] = {877,876,875,880}; AddFace(i, 4); }
{ int i[] = {891,892,889,890}; AddFace(i, 4); }
{ int i[] = {904,894,893,903}; AddFace(i, 4); }
{ int i[] = {895,893,894,896}; AddFace(i, 4); }
{ int i[] = {897,895,896,898}; AddFace(i, 4); }
{ int i[] = {899,897,898,900}; AddFace(i, 4); }
{ int i[] = {901,899,900,902}; AddFace(i, 4); }
{ int i[] = {904,903,901,902}; AddFace(i, 4); }
{ int i[] = {855,854,853,856}; AddFace(i, 4); }
{ int i[] = {852,997,996,851}; AddFace(i, 4); }
{ int i[] = {996,997,1009,1008}; AddFace(i, 4); }
{ int i[] = {853,998,997,852}; AddFace(i, 4); }
{ int i[] = {997,998,1010,1009}; AddFace(i, 4); }
{ int i[] = {853,854,999,998}; AddFace(i, 4); }
{ int i[] = {998,999,1011,1010}; AddFace(i, 4); }
{ int i[] = {854,855,1000,999}; AddFace(i, 4); }
{ int i[] = {999,1000,1012,1011}; AddFace(i, 4); }
{ int i[] = {855,856,1001,1000}; AddFace(i, 4); }
{ int i[] = {1000,1001,1013,1012}; AddFace(i, 4); }
{ int i[] = {851,996,1001,856}; AddFace(i, 4); }
{ int i[] = {1001,996,1008,1013}; AddFace(i, 4); }
{ int i[] = {858,857,869,870}; AddFace(i, 4); }
{ int i[] = {859,858,870,871}; AddFace(i, 4); }
{ int i[] = {860,859,871,872}; AddFace(i, 4); }
{ int i[] = {861,860,872,873}; AddFace(i, 4); }
{ int i[] = {862,861,873,874}; AddFace(i, 4); }
{ int i[] = {869,857,862,874}; AddFace(i, 4); }
{ int i[] = {857,858,1003,1002}; AddFace(i, 4); }
{ int i[] = {863,1002,1003,864}; AddFace(i, 4); }
{ int i[] = {858,859,1004,1003}; AddFace(i, 4); }
{ int i[] = {864,1003,1004,865}; AddFace(i, 4); }
{ int i[] = {859,860,1005,1004}; AddFace(i, 4); }
{ int i[] = {865,1004,1005,866}; AddFace(i, 4); }
{ int i[] = {860,861,1006,1005}; AddFace(i, 4); }
{ int i[] = {866,1005,1006,867}; AddFace(i, 4); }
{ int i[] = {861,862,1007,1006}; AddFace(i, 4); }
{ int i[] = {867,1006,1007,868}; AddFace(i, 4); }
{ int i[] = {862,857,1002,1007}; AddFace(i, 4); }
{ int i[] = {868,1007,1002,863}; AddFace(i, 4); }
{ int i[] = {868,863,864,865}; AddFace(i, 4); }
{ int i[] = {853,852,851,856}; AddFace(i, 4); }
{ int i[] = {867,868,865,866}; AddFace(i, 4); }
{ int i[] = {1008,869,874,1013}; AddFace(i, 4); }
{ int i[] = {870,869,1008,1009}; AddFace(i, 4); }
{ int i[] = {871,870,1009,1010}; AddFace(i, 4); }
{ int i[] = {872,871,1010,1011}; AddFace(i, 4); }
{ int i[] = {873,872,1011,1012}; AddFace(i, 4); }
{ int i[] = {874,873,1012,1013}; AddFace(i, 4); }
{ int i[] = {1014,1015,1017,1016}; AddFace(i, 4); }
{ int i[] = {1046,1047,1048,1049}; AddFace(i, 4); }
{ int i[] = {1018,1019,1021,1020}; AddFace(i, 4); }
{ int i[] = {1015,1021,1019,1017}; AddFace(i, 4); }
{ int i[] = {1020,1014,1016,1018}; AddFace(i, 4); }
{ int i[] = {1016,1017,1023,1022}; AddFace(i, 4); }
{ int i[] = {1017,1019,1024,1023}; AddFace(i, 4); }
{ int i[] = {1019,1018,1025,1024}; AddFace(i, 4); }
{ int i[] = {1018,1016,1022,1025}; AddFace(i, 4); }
{ int i[] = {1022,1023,1027,1026}; AddFace(i, 4); }
{ int i[] = {1023,1024,1028,1027}; AddFace(i, 4); }
{ int i[] = {1024,1025,1029,1028}; AddFace(i, 4); }
{ int i[] = {1025,1022,1026,1029}; AddFace(i, 4); }
{ int i[] = {1026,1027,1031,1030}; AddFace(i, 4); }
{ int i[] = {1027,1028,1032,1031}; AddFace(i, 4); }
{ int i[] = {1028,1029,1033,1032}; AddFace(i, 4); }
{ int i[] = {1029,1026,1030,1033}; AddFace(i, 4); }
{ int i[] = {1030,1031,1035,1034}; AddFace(i, 4); }
{ int i[] = {1031,1032,1036,1035}; AddFace(i, 4); }
{ int i[] = {1032,1033,1037,1036}; AddFace(i, 4); }
{ int i[] = {1033,1030,1034,1037}; AddFace(i, 4); }
{ int i[] = {1034,1035,1039,1038}; AddFace(i, 4); }
{ int i[] = {1035,1036,1040,1039}; AddFace(i, 4); }
{ int i[] = {1036,1037,1041,1040}; AddFace(i, 4); }
{ int i[] = {1037,1034,1038,1041}; AddFace(i, 4); }
{ int i[] = {1038,1039,1043,1042}; AddFace(i, 4); }
{ int i[] = {1039,1040,1044,1043}; AddFace(i, 4); }
{ int i[] = {1040,1041,1045,1044}; AddFace(i, 4); }
{ int i[] = {1041,1038,1042,1045}; AddFace(i, 4); }
{ int i[] = {1050,1051,1047,1046}; AddFace(i, 4); }
{ int i[] = {1051,1052,1048,1047}; AddFace(i, 4); }
{ int i[] = {1052,1053,1049,1048}; AddFace(i, 4); }
{ int i[] = {1053,1050,1046,1049}; AddFace(i, 4); }
{ int i[] = {1042,1043,1051,1050}; AddFace(i, 4); }
{ int i[] = {1043,1044,1052,1051}; AddFace(i, 4); }
{ int i[] = {1044,1045,1053,1052}; AddFace(i, 4); }
{ int i[] = {1045,1042,1050,1053}; AddFace(i, 4); }
{ int i[] = {1070,1067,1068,1069}; AddFace(i, 4); }
{ int i[] = {1054,1063,1062,1055}; AddFace(i, 4); }
{ int i[] = {1054,1055,1065,1064}; AddFace(i, 4); }
{ int i[] = {1055,1056,1066,1065}; AddFace(i, 4); }
{ int i[] = {1056,1057,1067,1066}; AddFace(i, 4); }
{ int i[] = {1057,1058,1068,1067}; AddFace(i, 4); }
{ int i[] = {1059,1060,1070,1069}; AddFace(i, 4); }
{ int i[] = {1060,1061,1071,1070}; AddFace(i, 4); }
{ int i[] = {1061,1062,1072,1071}; AddFace(i, 4); }
{ int i[] = {1062,1063,1073,1072}; AddFace(i, 4); }
{ int i[] = {1056,1061,1060,1057}; AddFace(i, 4); }
{ int i[] = {1055,1062,1061,1056}; AddFace(i, 4); }
{ int i[] = {1057,1060,1059,1058}; AddFace(i, 4); }
{ int i[] = {1073,1064,1065,1072}; AddFace(i, 4); }
{ int i[] = {1071,1066,1067,1070}; AddFace(i, 4); }
{ int i[] = {1072,1065,1066,1071}; AddFace(i, 4); }
{ int i[] = {1075,1074,1076,1077}; AddFace(i, 4); }
{ int i[] = {1078,1079,1080,1081}; AddFace(i, 4); }
{ int i[] = {1074,1080,1079,1076}; AddFace(i, 4); }
{ int i[] = {1079,1078,1077,1076}; AddFace(i, 4); }
{ int i[] = {1078,1081,1075,1077}; AddFace(i, 4); }
{ int i[] = {1081,1080,1074,1075}; AddFace(i, 4); }
{ int i[] = {1098,1095,1096,1097}; AddFace(i, 4); }
{ int i[] = {1082,1091,1090,1083}; AddFace(i, 4); }
{ int i[] = {1082,1083,1093,1092}; AddFace(i, 4); }
{ int i[] = {1083,1084,1094,1093}; AddFace(i, 4); }
{ int i[] = {1084,1085,1095,1094}; AddFace(i, 4); }
{ int i[] = {1085,1086,1096,1095}; AddFace(i, 4); }
{ int i[] = {1087,1088,1098,1097}; AddFace(i, 4); }
{ int i[] = {1088,1089,1099,1098}; AddFace(i, 4); }
{ int i[] = {1089,1090,1100,1099}; AddFace(i, 4); }
{ int i[] = {1090,1091,1101,1100}; AddFace(i, 4); }
{ int i[] = {1084,1089,1088,1085}; AddFace(i, 4); }
{ int i[] = {1083,1090,1089,1084}; AddFace(i, 4); }
{ int i[] = {1085,1088,1087,1086}; AddFace(i, 4); }
{ int i[] = {1101,1092,1093,1100}; AddFace(i, 4); }
{ int i[] = {1099,1094,1095,1098}; AddFace(i, 4); }
{ int i[] = {1100,1093,1094,1099}; AddFace(i, 4); }
{ int i[] = {1103,1102,1104,1105}; AddFace(i, 4); }
{ int i[] = {1106,1107,1108,1109}; AddFace(i, 4); }
{ int i[] = {1102,1108,1107,1104}; AddFace(i, 4); }
{ int i[] = {1107,1106,1105,1104}; AddFace(i, 4); }
{ int i[] = {1106,1109,1103,1105}; AddFace(i, 4); }
{ int i[] = {1109,1108,1102,1103}; AddFace(i, 4); }
{ int i[] = {1126,1123,1124,1125}; AddFace(i, 4); }
{ int i[] = {1110,1119,1118,1111}; AddFace(i, 4); }
{ int i[] = {1110,1111,1121,1120}; AddFace(i, 4); }
{ int i[] = {1111,1112,1122,1121}; AddFace(i, 4); }
{ int i[] = {1112,1113,1123,1122}; AddFace(i, 4); }
{ int i[] = {1113,1114,1124,1123}; AddFace(i, 4); }
{ int i[] = {1115,1116,1126,1125}; AddFace(i, 4); }
{ int i[] = {1116,1117,1127,1126}; AddFace(i, 4); }
{ int i[] = {1117,1118,1128,1127}; AddFace(i, 4); }
{ int i[] = {1118,1119,1129,1128}; AddFace(i, 4); }
{ int i[] = {1112,1117,1116,1113}; AddFace(i, 4); }
{ int i[] = {1111,1118,1117,1112}; AddFace(i, 4); }
{ int i[] = {1113,1116,1115,1114}; AddFace(i, 4); }
{ int i[] = {1129,1120,1121,1128}; AddFace(i, 4); }
{ int i[] = {1127,1122,1123,1126}; AddFace(i, 4); }
{ int i[] = {1128,1121,1122,1127}; AddFace(i, 4); }
{ int i[] = {1131,1130,1132,1133}; AddFace(i, 4); }
{ int i[] = {1134,1135,1136,1137}; AddFace(i, 4); }
{ int i[] = {1130,1136,1135,1132}; AddFace(i, 4); }
{ int i[] = {1135,1134,1133,1132}; AddFace(i, 4); }
{ int i[] = {1134,1137,1131,1133}; AddFace(i, 4); }
{ int i[] = {1137,1136,1130,1131}; AddFace(i, 4); }
{ int i[] = {1202,1204,1195,1197}; AddFace(i, 4); }
{ int i[] = {1206,1202,1197,1201}; AddFace(i, 4); }
{ int i[] = {1139,1162,1165,1147}; AddFace(i, 4); }
{ int i[] = {1164,1163,1140,1148}; AddFace(i, 4); }
{ int i[] = {1140,1141,1149,1148}; AddFace(i, 4); }
{ int i[] = {1141,1142,1150,1149}; AddFace(i, 4); }
{ int i[] = {1142,1143,1151,1150}; AddFace(i, 4); }
{ int i[] = {1143,1166,1169,1151}; AddFace(i, 4); }
{ int i[] = {1168,1167,1144,1152}; AddFace(i, 4); }
{ int i[] = {1195,1204,1208,1199}; AddFace(i, 4); }
{ int i[] = {1194,1198,1156,1161}; AddFace(i, 4); }
{ int i[] = {1145,1138,1146,1153}; AddFace(i, 4); }
{ int i[] = {1196,1194,1161,1159}; AddFace(i, 4); }
{ int i[] = {1169,1193,1150,1151}; AddFace(i, 4); }
{ int i[] = {1164,1192,1147,1165}; AddFace(i, 4); }
{ int i[] = {1152,1147,1192,1193}; AddFace(i, 4); }
{ int i[] = {1173,1190,1170,1172}; AddFace(i, 4); }
{ int i[] = {1176,1191,1178,1179}; AddFace(i, 4); }
{ int i[] = {1176,1170,1190,1191}; AddFace(i, 4); }
{ int i[] = {1199,1208,1206,1201}; AddFace(i, 4); }
{ int i[] = {1147,1152,1205,1203}; AddFace(i, 4); }
{ int i[] = {1155,1154,1156,1157}; AddFace(i, 4); }
{ int i[] = {1158,1155,1157,1159}; AddFace(i, 4); }
{ int i[] = {1139,1147,1203,1207}; AddFace(i, 4); }
{ int i[] = {1152,1144,1209,1205}; AddFace(i, 4); }
{ int i[] = {1154,1160,1161,1156}; AddFace(i, 4); }
{ int i[] = {1160,1158,1159,1161}; AddFace(i, 4); }
{ int i[] = {1144,1139,1207,1209}; AddFace(i, 4); }
{ int i[] = {1138,1145,1154,1155}; AddFace(i, 4); }
{ int i[] = {1198,1200,1157,1156}; AddFace(i, 4); }
{ int i[] = {1146,1138,1155,1158}; AddFace(i, 4); }
{ int i[] = {1200,1196,1159,1157}; AddFace(i, 4); }
{ int i[] = {1145,1153,1160,1154}; AddFace(i, 4); }
{ int i[] = {1153,1146,1158,1160}; AddFace(i, 4); }
{ int i[] = {1162,1163,1164,1165}; AddFace(i, 4); }
{ int i[] = {1166,1167,1168,1169}; AddFace(i, 4); }
{ int i[] = {1184,1185,1172,1170}; AddFace(i, 4); }
{ int i[] = {1185,1186,1173,1172}; AddFace(i, 4); }
{ int i[] = {1186,1187,1174,1173}; AddFace(i, 4); }
{ int i[] = {1187,1188,1171,1174}; AddFace(i, 4); }
{ int i[] = {1189,1180,1177,1175}; AddFace(i, 4); }
{ int i[] = {1180,1181,1178,1177}; AddFace(i, 4); }
{ int i[] = {1181,1182,1179,1178}; AddFace(i, 4); }
{ int i[] = {1182,1183,1176,1179}; AddFace(i, 4); }
{ int i[] = {1183,1184,1170,1176}; AddFace(i, 4); }
{ int i[] = {1188,1189,1175,1171}; AddFace(i, 4); }
{ int i[] = {1140,1163,1181,1180}; AddFace(i, 4); }
{ int i[] = {1163,1162,1182,1181}; AddFace(i, 4); }
{ int i[] = {1162,1139,1183,1182}; AddFace(i, 4); }
{ int i[] = {1139,1144,1184,1183}; AddFace(i, 4); }
{ int i[] = {1144,1167,1185,1184}; AddFace(i, 4); }
{ int i[] = {1167,1166,1186,1185}; AddFace(i, 4); }
{ int i[] = {1166,1143,1187,1186}; AddFace(i, 4); }
{ int i[] = {1143,1142,1188,1187}; AddFace(i, 4); }
{ int i[] = {1142,1141,1189,1188}; AddFace(i, 4); }
{ int i[] = {1141,1140,1180,1189}; AddFace(i, 4); }
{ int i[] = {1191,1190,1171,1175}; AddFace(i, 4); }
{ int i[] = {1178,1191,1175,1177}; AddFace(i, 4); }
{ int i[] = {1171,1190,1173,1174}; AddFace(i, 4); }
{ int i[] = {1193,1192,1149,1150}; AddFace(i, 4); }
{ int i[] = {1149,1192,1164,1148}; AddFace(i, 4); }
{ int i[] = {1152,1193,1169,1168}; AddFace(i, 4); }
{ int i[] = {1197,1195,1194,1196}; AddFace(i, 4); }
{ int i[] = {1198,1194,1195,1199}; AddFace(i, 4); }
{ int i[] = {1200,1201,1197,1196}; AddFace(i, 4); }
{ int i[] = {1200,1198,1199,1201}; AddFace(i, 4); }
{ int i[] = {1203,1202,1206,1207}; AddFace(i, 4); }
{ int i[] = {1202,1203,1205,1204}; AddFace(i, 4); }
{ int i[] = {1204,1205,1209,1208}; AddFace(i, 4); }
{ int i[] = {1207,1206,1208,1209}; AddFace(i, 4); }
{ int i[] = {1210,1211,1217,1216}; AddFace(i, 4); }
{ int i[] = {1211,1212,1218,1217}; AddFace(i, 4); }
{ int i[] = {1212,1213,1219,1218}; AddFace(i, 4); }
{ int i[] = {1213,1214,1220,1219}; AddFace(i, 4); }
{ int i[] = {1214,1215,1221,1220}; AddFace(i, 4); }
{ int i[] = {1215,1210,1216,1221}; AddFace(i, 4); }
{ int i[] = {1212,1215,1214,1213}; AddFace(i, 4); }
{ int i[] = {1234,1235,1236,1237}; AddFace(i, 4); }
{ int i[] = {1238,1239,1240,1241}; AddFace(i, 4); }
{ int i[] = {1242,1243,1244,1245}; AddFace(i, 4); }
{ int i[] = {1222,1225,1224,1223}; AddFace(i, 4); }
{ int i[] = {1222,1223,1235,1234}; AddFace(i, 4); }
{ int i[] = {1223,1224,1236,1235}; AddFace(i, 4); }
{ int i[] = {1224,1225,1237,1236}; AddFace(i, 4); }
{ int i[] = {1225,1222,1234,1237}; AddFace(i, 4); }
{ int i[] = {1226,1229,1228,1227}; AddFace(i, 4); }
{ int i[] = {1226,1227,1239,1238}; AddFace(i, 4); }
{ int i[] = {1227,1228,1240,1239}; AddFace(i, 4); }
{ int i[] = {1228,1229,1241,1240}; AddFace(i, 4); }
{ int i[] = {1229,1226,1238,1241}; AddFace(i, 4); }
{ int i[] = {1230,1233,1232,1231}; AddFace(i, 4); }
{ int i[] = {1230,1231,1243,1242}; AddFace(i, 4); }
{ int i[] = {1231,1232,1244,1243}; AddFace(i, 4); }
{ int i[] = {1232,1233,1245,1244}; AddFace(i, 4); }
{ int i[] = {1233,1230,1242,1245}; AddFace(i, 4); }
{ int i[] = {1216,1217,1247,1246}; AddFace(i, 4); }
{ int i[] = {1217,1218,1248,1247}; AddFace(i, 4); }
{ int i[] = {1218,1219,1249,1248}; AddFace(i, 4); }
{ int i[] = {1219,1220,1250,1249}; AddFace(i, 4); }
{ int i[] = {1220,1221,1251,1250}; AddFace(i, 4); }
{ int i[] = {1221,1216,1246,1251}; AddFace(i, 4); }
{ int i[] = {1336,1337,1338,1339}; AddFace(i, 4); }
{ int i[] = {1337,1340,1341,1338}; AddFace(i, 4); }
{ int i[] = {1340,1342,1343,1341}; AddFace(i, 4); }
{ int i[] = {1342,1344,1345,1343}; AddFace(i, 4); }
{ int i[] = {1344,1346,1347,1345}; AddFace(i, 4); }
{ int i[] = {1346,1336,1339,1347}; AddFace(i, 4); }
{ int i[] = {1264,1265,1259,1258}; AddFace(i, 4); }
{ int i[] = {1265,1266,1260,1259}; AddFace(i, 4); }
{ int i[] = {1266,1267,1261,1260}; AddFace(i, 4); }
{ int i[] = {1267,1268,1262,1261}; AddFace(i, 4); }
{ int i[] = {1268,1269,1263,1262}; AddFace(i, 4); }
{ int i[] = {1269,1264,1258,1263}; AddFace(i, 4); }
{ int i[] = {1252,1253,1265,1264}; AddFace(i, 4); }
{ int i[] = {1253,1254,1266,1265}; AddFace(i, 4); }
{ int i[] = {1254,1255,1267,1266}; AddFace(i, 4); }
{ int i[] = {1255,1256,1268,1267}; AddFace(i, 4); }
{ int i[] = {1256,1257,1269,1268}; AddFace(i, 4); }
{ int i[] = {1257,1252,1264,1269}; AddFace(i, 4); }
{ int i[] = {1271,1270,1282,1283}; AddFace(i, 4); }
{ int i[] = {1272,1271,1283,1284}; AddFace(i, 4); }
{ int i[] = {1273,1272,1284,1285}; AddFace(i, 4); }
{ int i[] = {1274,1273,1285,1286}; AddFace(i, 4); }
{ int i[] = {1275,1274,1286,1287}; AddFace(i, 4); }
{ int i[] = {1276,1275,1287,1288}; AddFace(i, 4); }
{ int i[] = {1277,1276,1288,1289}; AddFace(i, 4); }
{ int i[] = {1278,1277,1289,1290}; AddFace(i, 4); }
{ int i[] = {1279,1278,1290,1291}; AddFace(i, 4); }
{ int i[] = {1280,1279,1291,1292}; AddFace(i, 4); }
{ int i[] = {1281,1280,1292,1293}; AddFace(i, 4); }
{ int i[] = {1270,1281,1293,1282}; AddFace(i, 4); }
{ int i[] = {1283,1282,1294,1295}; AddFace(i, 4); }
{ int i[] = {1284,1283,1295,1296}; AddFace(i, 4); }
{ int i[] = {1285,1284,1296,1297}; AddFace(i, 4); }
{ int i[] = {1286,1285,1297,1298}; AddFace(i, 4); }
{ int i[] = {1287,1286,1298,1299}; AddFace(i, 4); }
{ int i[] = {1288,1287,1299,1300}; AddFace(i, 4); }
{ int i[] = {1289,1288,1300,1301}; AddFace(i, 4); }
{ int i[] = {1290,1289,1301,1302}; AddFace(i, 4); }
{ int i[] = {1291,1290,1302,1303}; AddFace(i, 4); }
{ int i[] = {1292,1291,1303,1304}; AddFace(i, 4); }
{ int i[] = {1293,1292,1304,1305}; AddFace(i, 4); }
{ int i[] = {1282,1293,1305,1294}; AddFace(i, 4); }
{ int i[] = {1295,1294,1306,1307}; AddFace(i, 4); }
{ int i[] = {1296,1295,1307,1308}; AddFace(i, 4); }
{ int i[] = {1297,1296,1308,1309}; AddFace(i, 4); }
{ int i[] = {1298,1297,1309,1310}; AddFace(i, 4); }
{ int i[] = {1299,1298,1310,1311}; AddFace(i, 4); }
{ int i[] = {1300,1299,1311,1312}; AddFace(i, 4); }
{ int i[] = {1301,1300,1312,1313}; AddFace(i, 4); }
{ int i[] = {1302,1301,1313,1314}; AddFace(i, 4); }
{ int i[] = {1303,1302,1314,1315}; AddFace(i, 4); }
{ int i[] = {1304,1303,1315,1316}; AddFace(i, 4); }
{ int i[] = {1305,1304,1316,1317}; AddFace(i, 4); }
{ int i[] = {1294,1305,1317,1306}; AddFace(i, 4); }
{ int i[] = {1307,1306,1318,1319}; AddFace(i, 4); }
{ int i[] = {1308,1307,1319,1320}; AddFace(i, 4); }
{ int i[] = {1309,1308,1320,1321}; AddFace(i, 4); }
{ int i[] = {1310,1309,1321,1322}; AddFace(i, 4); }
{ int i[] = {1311,1310,1322,1323}; AddFace(i, 4); }
{ int i[] = {1312,1311,1323,1324}; AddFace(i, 4); }
{ int i[] = {1313,1312,1324,1325}; AddFace(i, 4); }
{ int i[] = {1314,1313,1325,1326}; AddFace(i, 4); }
{ int i[] = {1315,1314,1326,1327}; AddFace(i, 4); }
{ int i[] = {1316,1315,1327,1328}; AddFace(i, 4); }
{ int i[] = {1317,1316,1328,1329}; AddFace(i, 4); }
{ int i[] = {1306,1317,1329,1318}; AddFace(i, 4); }
{ int i[] = {1319,1318,1270,1271}; AddFace(i, 4); }
{ int i[] = {1320,1319,1271,1272}; AddFace(i, 4); }
{ int i[] = {1321,1320,1272,1273}; AddFace(i, 4); }
{ int i[] = {1322,1321,1273,1274}; AddFace(i, 4); }
{ int i[] = {1323,1322,1274,1275}; AddFace(i, 4); }
{ int i[] = {1324,1323,1275,1276}; AddFace(i, 4); }
{ int i[] = {1325,1324,1276,1277}; AddFace(i, 4); }
{ int i[] = {1326,1325,1277,1278}; AddFace(i, 4); }
{ int i[] = {1327,1326,1278,1279}; AddFace(i, 4); }
{ int i[] = {1328,1327,1279,1280}; AddFace(i, 4); }
{ int i[] = {1329,1328,1280,1281}; AddFace(i, 4); }
{ int i[] = {1318,1329,1281,1270}; AddFace(i, 4); }
{ int i[] = {1249,1250,1331,1330}; AddFace(i, 4); }
{ int i[] = {1250,1251,1332,1331}; AddFace(i, 4); }
{ int i[] = {1251,1246,1333,1332}; AddFace(i, 4); }
{ int i[] = {1246,1247,1334,1333}; AddFace(i, 4); }
{ int i[] = {1247,1248,1335,1334}; AddFace(i, 4); }
{ int i[] = {1248,1249,1330,1335}; AddFace(i, 4); }
{ int i[] = {1333,1334,1337,1336}; AddFace(i, 4); }
{ int i[] = {1253,1252,1339,1338}; AddFace(i, 4); }
{ int i[] = {1334,1335,1340,1337}; AddFace(i, 4); }
{ int i[] = {1254,1253,1338,1341}; AddFace(i, 4); }
{ int i[] = {1335,1330,1342,1340}; AddFace(i, 4); }
{ int i[] = {1255,1254,1341,1343}; AddFace(i, 4); }
{ int i[] = {1330,1331,1344,1342}; AddFace(i, 4); }
{ int i[] = {1256,1255,1343,1345}; AddFace(i, 4); }
{ int i[] = {1331,1332,1346,1344}; AddFace(i, 4); }
{ int i[] = {1257,1256,1345,1347}; AddFace(i, 4); }
{ int i[] = {1332,1333,1336,1346}; AddFace(i, 4); }
{ int i[] = {1252,1257,1347,1339}; AddFace(i, 4); }
{ int i[] = {1210,1215,1212,1211}; AddFace(i, 4); }
{ int i[] = {1348,1349,1351,1350}; AddFace(i, 4); }
{ int i[] = {1350,1351,1353,1352}; AddFace(i, 4); }
{ int i[] = {1352,1353,1355,1354}; AddFace(i, 4); }
{ int i[] = {1349,1355,1353,1351}; AddFace(i, 4); }
{ int i[] = {1354,1348,1350,1352}; AddFace(i, 4); }
{ int i[] = {1356,1357,1359,1358}; AddFace(i, 4); }
{ int i[] = {1358,1359,1361,1360}; AddFace(i, 4); }
{ int i[] = {1360,1361,1363,1362}; AddFace(i, 4); }
{ int i[] = {1362,1363,1357,1356}; AddFace(i, 4); }
{ int i[] = {1357,1363,1361,1359}; AddFace(i, 4); }
{ int i[] = {1362,1356,1358,1360}; AddFace(i, 4); }
{ int i[] = {1406,1380,1381,1382}; AddFace(i, 4); }
{ int i[] = {1407,1364,1380,1406}; AddFace(i, 4); }
{ int i[] = {1364,1365,1381,1380}; AddFace(i, 4); }
{ int i[] = {1365,1366,1382,1381}; AddFace(i, 4); }
{ int i[] = {1366,1367,1383,1382}; AddFace(i, 4); }
{ int i[] = {1367,1437,1438,1383}; AddFace(i, 4); }
{ int i[] = {1439,1436,1368,1384}; AddFace(i, 4); }
{ int i[] = {1368,1369,1385,1384}; AddFace(i, 4); }
{ int i[] = {1369,1370,1386,1385}; AddFace(i, 4); }
{ int i[] = {1370,1371,1387,1386}; AddFace(i, 4); }
{ int i[] = {1371,1372,1388,1387}; AddFace(i, 4); }
{ int i[] = {1372,1373,1389,1388}; AddFace(i, 4); }
{ int i[] = {1373,1374,1390,1389}; AddFace(i, 4); }
{ int i[] = {1374,1375,1391,1390}; AddFace(i, 4); }
{ int i[] = {1375,1376,1392,1391}; AddFace(i, 4); }
{ int i[] = {1376,1377,1393,1392}; AddFace(i, 4); }
{ int i[] = {1411,1378,1394,1410}; AddFace(i, 4); }
{ int i[] = {1397,1379,1395,1396}; AddFace(i, 4); }
{ int i[] = {1403,1398,1400,1402}; AddFace(i, 4); }
{ int i[] = {1394,1390,1391,1410}; AddFace(i, 4); }
{ int i[] = {1404,1405,1443,1440}; AddFace(i, 4); }
{ int i[] = {1395,1388,1389,1396}; AddFace(i, 4); }
{ int i[] = {1396,1389,1390,1394}; AddFace(i, 4); }
{ int i[] = {1378,1397,1396,1394}; AddFace(i, 4); }
{ int i[] = {1400,1401,1385,1386}; AddFace(i, 4); }
{ int i[] = {1398,1399,1401,1400}; AddFace(i, 4); }
{ int i[] = {1400,1386,1387,1402}; AddFace(i, 4); }
{ int i[] = {1384,1404,1440,1439}; AddFace(i, 4); }
{ int i[] = {1402,1387,1388,1395}; AddFace(i, 4); }
{ int i[] = {1379,1403,1402,1395}; AddFace(i, 4); }
{ int i[] = {1385,1401,1404,1384}; AddFace(i, 4); }
{ int i[] = {1401,1399,1405,1404}; AddFace(i, 4); }
{ int i[] = {1438,1441,1409,1383}; AddFace(i, 4); }
{ int i[] = {1441,1442,1408,1409}; AddFace(i, 4); }
{ int i[] = {1408,1407,1406,1409}; AddFace(i, 4); }
{ int i[] = {1383,1409,1406,1382}; AddFace(i, 4); }
{ int i[] = {1410,1391,1392,1393}; AddFace(i, 4); }
{ int i[] = {1377,1411,1410,1393}; AddFace(i, 4); }
{ int i[] = {1412,1415,1414,1413}; AddFace(i, 4); }
{ int i[] = {1407,1412,1413,1364}; AddFace(i, 4); }
{ int i[] = {1364,1413,1414,1365}; AddFace(i, 4); }
{ int i[] = {1365,1414,1415,1366}; AddFace(i, 4); }
{ int i[] = {1366,1415,1416,1367}; AddFace(i, 4); }
{ int i[] = {1367,1416,1445,1437}; AddFace(i, 4); }
{ int i[] = {1436,1444,1417,1368}; AddFace(i, 4); }
{ int i[] = {1368,1417,1418,1369}; AddFace(i, 4); }
{ int i[] = {1369,1418,1419,1370}; AddFace(i, 4); }
{ int i[] = {1370,1419,1420,1371}; AddFace(i, 4); }
{ int i[] = {1371,1420,1421,1372}; AddFace(i, 4); }
{ int i[] = {1372,1421,1422,1373}; AddFace(i, 4); }
{ int i[] = {1373,1422,1423,1374}; AddFace(i, 4); }
{ int i[] = {1374,1423,1424,1375}; AddFace(i, 4); }
{ int i[] = {1375,1424,1425,1376}; AddFace(i, 4); }
{ int i[] = {1376,1425,1426,1377}; AddFace(i, 4); }
{ int i[] = {1411,1428,1427,1378}; AddFace(i, 4); }
{ int i[] = {1397,1430,1429,1379}; AddFace(i, 4); }
{ int i[] = {1403,1432,1431,1398}; AddFace(i, 4); }
{ int i[] = {1427,1428,1424,1423}; AddFace(i, 4); }
{ int i[] = {1433,1447,1443,1405}; AddFace(i, 4); }
{ int i[] = {1429,1430,1422,1421}; AddFace(i, 4); }
{ int i[] = {1430,1427,1423,1422}; AddFace(i, 4); }
{ int i[] = {1378,1427,1430,1397}; AddFace(i, 4); }
{ int i[] = {1431,1419,1418,1434}; AddFace(i, 4); }
{ int i[] = {1398,1431,1434,1399}; AddFace(i, 4); }
{ int i[] = {1431,1432,1420,1419}; AddFace(i, 4); }
{ int i[] = {1417,1444,1447,1433}; AddFace(i, 4); }
{ int i[] = {1432,1429,1421,1420}; AddFace(i, 4); }
{ int i[] = {1379,1429,1432,1403}; AddFace(i, 4); }
{ int i[] = {1418,1417,1433,1434}; AddFace(i, 4); }
{ int i[] = {1434,1433,1405,1399}; AddFace(i, 4); }
{ int i[] = {1446,1445,1416,1435}; AddFace(i, 4); }
{ int i[] = {1442,1446,1435,1408}; AddFace(i, 4); }
{ int i[] = {1408,1435,1412,1407}; AddFace(i, 4); }
{ int i[] = {1416,1415,1412,1435}; AddFace(i, 4); }
{ int i[] = {1428,1426,1425,1424}; AddFace(i, 4); }
{ int i[] = {1377,1426,1428,1411}; AddFace(i, 4); }
{ int i[] = {1444,1436,1443,1447}; AddFace(i, 4); }
{ int i[] = {1442,1437,1445,1446}; AddFace(i, 4); }
{ int i[] = {1438,1437,1442,1441}; AddFace(i, 4); }
{ int i[] = {1443,1436,1439,1440}; AddFace(i, 4); }
{ int i[] = {1448,1449,1451,1450}; AddFace(i, 4); }
{ int i[] = {1450,1451,1453,1452}; AddFace(i, 4); }
{ int i[] = {1452,1453,1455,1454}; AddFace(i, 4); }
{ int i[] = {1449,1455,1453,1451}; AddFace(i, 4); }
{ int i[] = {1454,1448,1450,1452}; AddFace(i, 4); }
{ int i[] = {1456,1457,1459,1458}; AddFace(i, 4); }
{ int i[] = {1458,1459,1461,1460}; AddFace(i, 4); }
{ int i[] = {1460,1461,1463,1462}; AddFace(i, 4); }
{ int i[] = {1462,1463,1457,1456}; AddFace(i, 4); }
{ int i[] = {1457,1463,1461,1459}; AddFace(i, 4); }
{ int i[] = {1462,1456,1458,1460}; AddFace(i, 4); }
{ int i[] = {1506,1480,1481,1482}; AddFace(i, 4); }
{ int i[] = {1507,1464,1480,1506}; AddFace(i, 4); }
{ int i[] = {1464,1465,1481,1480}; AddFace(i, 4); }
{ int i[] = {1465,1466,1482,1481}; AddFace(i, 4); }
{ int i[] = {1466,1467,1483,1482}; AddFace(i, 4); }
{ int i[] = {1467,1537,1538,1483}; AddFace(i, 4); }
{ int i[] = {1539,1536,1468,1484}; AddFace(i, 4); }
{ int i[] = {1468,1469,1485,1484}; AddFace(i, 4); }
{ int i[] = {1469,1470,1486,1485}; AddFace(i, 4); }
{ int i[] = {1470,1471,1487,1486}; AddFace(i, 4); }
{ int i[] = {1471,1472,1488,1487}; AddFace(i, 4); }
{ int i[] = {1472,1473,1489,1488}; AddFace(i, 4); }
{ int i[] = {1473,1474,1490,1489}; AddFace(i, 4); }
{ int i[] = {1474,1475,1491,1490}; AddFace(i, 4); }
{ int i[] = {1475,1476,1492,1491}; AddFace(i, 4); }
{ int i[] = {1476,1477,1493,1492}; AddFace(i, 4); }
{ int i[] = {1511,1478,1494,1510}; AddFace(i, 4); }
{ int i[] = {1497,1479,1495,1496}; AddFace(i, 4); }
{ int i[] = {1503,1498,1500,1502}; AddFace(i, 4); }
{ int i[] = {1494,1490,1491,1510}; AddFace(i, 4); }
{ int i[] = {1504,1505,1543,1540}; AddFace(i, 4); }
{ int i[] = {1495,1488,1489,1496}; AddFace(i, 4); }
{ int i[] = {1496,1489,1490,1494}; AddFace(i, 4); }
{ int i[] = {1478,1497,1496,1494}; AddFace(i, 4); }
{ int i[] = {1500,1501,1485,1486}; AddFace(i, 4); }
{ int i[] = {1498,1499,1501,1500}; AddFace(i, 4); }
{ int i[] = {1500,1486,1487,1502}; AddFace(i, 4); }
{ int i[] = {1484,1504,1540,1539}; AddFace(i, 4); }
{ int i[] = {1502,1487,1488,1495}; AddFace(i, 4); }
{ int i[] = {1479,1503,1502,1495}; AddFace(i, 4); }
{ int i[] = {1485,1501,1504,1484}; AddFace(i, 4); }
{ int i[] = {1501,1499,1505,1504}; AddFace(i, 4); }
{ int i[] = {1538,1541,1509,1483}; AddFace(i, 4); }
{ int i[] = {1541,1542,1508,1509}; AddFace(i, 4); }
{ int i[] = {1508,1507,1506,1509}; AddFace(i, 4); }
{ int i[] = {1483,1509,1506,1482}; AddFace(i, 4); }
{ int i[] = {1510,1491,1492,1493}; AddFace(i, 4); }
{ int i[] = {1477,1511,1510,1493}; AddFace(i, 4); }
{ int i[] = {1512,1515,1514,1513}; AddFace(i, 4); }
{ int i[] = {1507,1512,1513,1464}; AddFace(i, 4); }
{ int i[] = {1464,1513,1514,1465}; AddFace(i, 4); }
{ int i[] = {1465,1514,1515,1466}; AddFace(i, 4); }
{ int i[] = {1466,1515,1516,1467}; AddFace(i, 4); }
{ int i[] = {1467,1516,1545,1537}; AddFace(i, 4); }
{ int i[] = {1536,1544,1517,1468}; AddFace(i, 4); }
{ int i[] = {1468,1517,1518,1469}; AddFace(i, 4); }
{ int i[] = {1469,1518,1519,1470}; AddFace(i, 4); }
{ int i[] = {1470,1519,1520,1471}; AddFace(i, 4); }
{ int i[] = {1471,1520,1521,1472}; AddFace(i, 4); }
{ int i[] = {1472,1521,1522,1473}; AddFace(i, 4); }
{ int i[] = {1473,1522,1523,1474}; AddFace(i, 4); }
{ int i[] = {1474,1523,1524,1475}; AddFace(i, 4); }
{ int i[] = {1475,1524,1525,1476}; AddFace(i, 4); }
{ int i[] = {1476,1525,1526,1477}; AddFace(i, 4); }
{ int i[] = {1511,1528,1527,1478}; AddFace(i, 4); }
{ int i[] = {1497,1530,1529,1479}; AddFace(i, 4); }
{ int i[] = {1503,1532,1531,1498}; AddFace(i, 4); }
{ int i[] = {1527,1528,1524,1523}; AddFace(i, 4); }
{ int i[] = {1533,1547,1543,1505}; AddFace(i, 4); }
{ int i[] = {1529,1530,1522,1521}; AddFace(i, 4); }
{ int i[] = {1530,1527,1523,1522}; AddFace(i, 4); }
{ int i[] = {1478,1527,1530,1497}; AddFace(i, 4); }
{ int i[] = {1531,1519,1518,1534}; AddFace(i, 4); }
{ int i[] = {1498,1531,1534,1499}; AddFace(i, 4); }
{ int i[] = {1531,1532,1520,1519}; AddFace(i, 4); }
{ int i[] = {1517,1544,1547,1533}; AddFace(i, 4); }
{ int i[] = {1532,1529,1521,1520}; AddFace(i, 4); }
{ int i[] = {1479,1529,1532,1503}; AddFace(i, 4); }
{ int i[] = {1518,1517,1533,1534}; AddFace(i, 4); }
{ int i[] = {1534,1533,1505,1499}; AddFace(i, 4); }
{ int i[] = {1546,1545,1516,1535}; AddFace(i, 4); }
{ int i[] = {1542,1546,1535,1508}; AddFace(i, 4); }
{ int i[] = {1508,1535,1512,1507}; AddFace(i, 4); }
{ int i[] = {1516,1515,1512,1535}; AddFace(i, 4); }
{ int i[] = {1528,1526,1525,1524}; AddFace(i, 4); }
{ int i[] = {1477,1526,1528,1511}; AddFace(i, 4); }
{ int i[] = {1544,1536,1543,1547}; AddFace(i, 4); }
{ int i[] = {1542,1537,1545,1546}; AddFace(i, 4); }
{ int i[] = {1538,1537,1542,1541}; AddFace(i, 4); }
{ int i[] = {1543,1536,1539,1540}; AddFace(i, 4); }
{ int i[] = {1569,1571,1549,1548}; AddFace(i, 4); }
{ int i[] = {1571,1573,1550,1549}; AddFace(i, 4); }
{ int i[] = {1573,1574,1551,1550}; AddFace(i, 4); }
{ int i[] = {1577,1569,1548,1552}; AddFace(i, 4); }
{ int i[] = {1548,1549,1554,1553}; AddFace(i, 4); }
{ int i[] = {1549,1550,1555,1554}; AddFace(i, 4); }
{ int i[] = {1550,1551,1556,1555}; AddFace(i, 4); }
{ int i[] = {1552,1548,1553,1557}; AddFace(i, 4); }
{ int i[] = {1553,1554,1559,1558}; AddFace(i, 4); }
{ int i[] = {1554,1555,1560,1559}; AddFace(i, 4); }
{ int i[] = {1555,1556,1561,1560}; AddFace(i, 4); }
{ int i[] = {1557,1553,1558,1562}; AddFace(i, 4); }
{ int i[] = {1572,1570,1563,1564}; AddFace(i, 4); }
{ int i[] = {1575,1572,1564,1565}; AddFace(i, 4); }
{ int i[] = {1568,1576,1566,1567}; AddFace(i, 4); }
{ int i[] = {1570,1568,1567,1563}; AddFace(i, 4); }
{ int i[] = {1569,1568,1570,1571}; AddFace(i, 4); }
{ int i[] = {1568,1569,1577,1576}; AddFace(i, 4); }
{ int i[] = {1571,1570,1572,1573}; AddFace(i, 4); }
{ int i[] = {1573,1572,1575,1574}; AddFace(i, 4); }
{ int i[] = {1640,1639,1588,1597}; AddFace(i, 4); }
{ int i[] = {1600,1583,1584,1599}; AddFace(i, 4); }
{ int i[] = {1601,1582,1583,1600}; AddFace(i, 4); }
{ int i[] = {1601,1594,1593,1582}; AddFace(i, 4); }
{ int i[] = {1621,1620,1596,1586}; AddFace(i, 4); }
{ int i[] = {1593,1594,1595,1591}; AddFace(i, 4); }
{ int i[] = {1598,1599,1584,1585}; AddFace(i, 4); }
{ int i[] = {1594,1592,1590,1595}; AddFace(i, 4); }
{ int i[] = {1596,1620,1619,1587}; AddFace(i, 4); }
{ int i[] = {1641,1640,1597,1610}; AddFace(i, 4); }
{ int i[] = {1579,1578,1599,1598}; AddFace(i, 4); }
{ int i[] = {1580,1600,1599,1578}; AddFace(i, 4); }
{ int i[] = {1581,1601,1600,1580}; AddFace(i, 4); }
{ int i[] = {1581,1592,1594,1601}; AddFace(i, 4); }
{ int i[] = {1633,1622,1585,1584}; AddFace(i, 4); }
{ int i[] = {1602,1617,1616,1589}; AddFace(i, 4); }
{ int i[] = {1605,1618,1617,1602}; AddFace(i, 4); }
{ int i[] = {1580,1578,1579,1603}; AddFace(i, 4); }
{ int i[] = {1604,1581,1580,1603}; AddFace(i, 4); }
{ int i[] = {1587,1619,1618,1605}; AddFace(i, 4); }
{ int i[] = {1612,1597,1588,1613}; AddFace(i, 4); }
{ int i[] = {1606,1607,1605,1602}; AddFace(i, 4); }
{ int i[] = {1607,1609,1587,1605}; AddFace(i, 4); }
{ int i[] = {1608,1606,1602,1589}; AddFace(i, 4); }
{ int i[] = {1609,1586,1596,1587}; AddFace(i, 4); }
{ int i[] = {1604,1590,1592,1581}; AddFace(i, 4); }
{ int i[] = {1579,1641,1610,1616}; AddFace(i, 4); }
{ int i[] = {1613,1615,1614,1612}; AddFace(i, 4); }
{ int i[] = {1614,1610,1597,1612}; AddFace(i, 4); }
{ int i[] = {1589,1614,1615,1608}; AddFace(i, 4); }
{ int i[] = {1613,1588,1611,1615}; AddFace(i, 4); }
{ int i[] = {1589,1616,1610,1614}; AddFace(i, 4); }
{ int i[] = {1603,1579,1616,1617}; AddFace(i, 4); }
{ int i[] = {1618,1604,1603,1617}; AddFace(i, 4); }
{ int i[] = {1619,1590,1604,1618}; AddFace(i, 4); }
{ int i[] = {1620,1595,1590,1619}; AddFace(i, 4); }
{ int i[] = {1591,1595,1620,1621}; AddFace(i, 4); }
{ int i[] = {1631,1630,1623,1624}; AddFace(i, 4); }
{ int i[] = {1638,1611,1588,1639}; AddFace(i, 4); }
{ int i[] = {1635,1607,1606,1636}; AddFace(i, 4); }
{ int i[] = {1634,1625,1591,1621}; AddFace(i, 4); }
{ int i[] = {1627,1631,1624,1622}; AddFace(i, 4); }
{ int i[] = {1593,1591,1625,1632}; AddFace(i, 4); }
{ int i[] = {1609,1607,1635,1634}; AddFace(i, 4); }
{ int i[] = {1606,1608,1637,1636}; AddFace(i, 4); }
{ int i[] = {1611,1638,1622,1637}; AddFace(i, 4); }
{ int i[] = {1583,1582,1629,1628}; AddFace(i, 4); }
{ int i[] = {1625,1623,1630,1626}; AddFace(i, 4); }
{ int i[] = {1628,1629,1626,1627}; AddFace(i, 4); }
{ int i[] = {1627,1626,1630,1631}; AddFace(i, 4); }
{ int i[] = {1582,1593,1632,1629}; AddFace(i, 4); }
{ int i[] = {1625,1626,1629,1632}; AddFace(i, 4); }
{ int i[] = {1628,1627,1622,1633}; AddFace(i, 4); }
{ int i[] = {1583,1628,1633,1584}; AddFace(i, 4); }
{ int i[] = {1634,1635,1623,1625}; AddFace(i, 4); }
{ int i[] = {1623,1635,1636,1624}; AddFace(i, 4); }
{ int i[] = {1637,1622,1624,1636}; AddFace(i, 4); }
{ int i[] = {1634,1621,1586,1609}; AddFace(i, 4); }
{ int i[] = {1615,1611,1637,1608}; AddFace(i, 4); }
{ int i[] = {1622,1638,1639,1585}; AddFace(i, 4); }
{ int i[] = {1598,1585,1639,1640}; AddFace(i, 4); }
{ int i[] = {1579,1598,1640,1641}; AddFace(i, 4); }
std::vector<float> v;
v.push_back(1.205502);
v.push_back(-2.232923);
v.push_back(0.107551);
v.push_back(1.199445);
v.push_back(-2.274622);
v.push_back(0.230427);
v.push_back(1.209483);
v.push_back(-2.014354);
v.push_back(0.109709);
v.push_back(1.194877);
v.push_back(-1.645203);
v.push_back(0.112483);
v.push_back(1.142456);
v.push_back(-1.621897);
v.push_back(0.074964);
v.push_back(1.170278);
v.push_back(-2.042560);
v.push_back(0.069819);
v.push_back(1.176917);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(1.178464);
v.push_back(-2.323577);
v.push_back(0.209123);
v.push_back(1.176339);
v.push_back(-1.390671);
v.push_back(0.503985);
v.push_back(1.211871);
v.push_back(-1.439150);
v.push_back(0.493781);
v.push_back(1.123573);
v.push_back(-2.352190);
v.push_back(0.341155);
v.push_back(1.159160);
v.push_back(-2.212864);
v.push_back(0.340564);
v.push_back(1.234142);
v.push_back(-1.493204);
v.push_back(0.270655);
v.push_back(1.195693);
v.push_back(-1.453670);
v.push_back(0.254945);
v.push_back(1.195168);
v.push_back(-1.583237);
v.push_back(0.152629);
v.push_back(1.151828);
v.push_back(-1.562341);
v.push_back(0.123414);
v.push_back(1.196251);
v.push_back(-1.569560);
v.push_back(0.127032);
v.push_back(1.237352);
v.push_back(-1.461312);
v.push_back(0.260222);
v.push_back(1.213251);
v.push_back(-1.400697);
v.push_back(0.508775);
v.push_back(1.135495);
v.push_back(-2.350801);
v.push_back(0.349428);
v.push_back(1.193862);
v.push_back(-2.317602);
v.push_back(0.211766);
v.push_back(1.197520);
v.push_back(-2.275416);
v.push_back(0.074770);
v.push_back(1.196322);
v.push_back(-2.039831);
v.push_back(0.078120);
v.push_back(1.191684);
v.push_back(-1.629162);
v.push_back(0.080931);
v.push_back(1.184448);
v.push_back(-1.936357);
v.push_back(0.361076);
v.push_back(1.232681);
v.push_back(-1.964357);
v.push_back(0.240676);
v.push_back(1.246753);
v.push_back(-1.652524);
v.push_back(0.253759);
v.push_back(1.196099);
v.push_back(-1.661617);
v.push_back(0.406213);
v.push_back(1.163565);
v.push_back(-1.933941);
v.push_back(0.362016);
v.push_back(1.178416);
v.push_back(-1.655905);
v.push_back(0.403308);
v.push_back(1.135195);
v.push_back(-2.216333);
v.push_back(0.342988);
v.push_back(1.176834);
v.push_back(-1.435313);
v.push_back(0.487460);
v.push_back(1.149304);
v.push_back(-2.283690);
v.push_back(0.340778);
v.push_back(1.131611);
v.push_back(-2.285208);
v.push_back(0.337738);
v.push_back(1.111566);
v.push_back(-2.281246);
v.push_back(0.400629);
v.push_back(1.094504);
v.push_back(-2.282764);
v.push_back(0.395046);
v.push_back(1.140173);
v.push_back(-2.258845);
v.push_back(0.365851);
v.push_back(1.116678);
v.push_back(-2.260331);
v.push_back(0.359688);
v.push_back(1.167562);
v.push_back(-2.220509);
v.push_back(0.326930);
v.push_back(1.191177);
v.push_back(-1.939823);
v.push_back(0.346171);
v.push_back(1.203127);
v.push_back(-1.660491);
v.push_back(0.387340);
v.push_back(1.218043);
v.push_back(-1.445842);
v.push_back(0.466160);
v.push_back(1.216234);
v.push_back(-1.408201);
v.push_back(0.478006);
v.push_back(1.178735);
v.push_back(-1.398470);
v.push_back(0.473155);
v.push_back(1.164098);
v.push_back(-2.253986);
v.push_back(0.214456);
v.push_back(1.181132);
v.push_back(-1.677794);
v.push_back(0.244248);
v.push_back(1.161505);
v.push_back(-1.981175);
v.push_back(0.230509);
v.push_back(1.190102);
v.push_back(-1.539699);
v.push_back(0.250825);
v.push_back(1.112837);
v.push_back(-1.649877);
v.push_back(0.212224);
v.push_back(1.116935);
v.push_back(-2.012577);
v.push_back(0.203145);
v.push_back(1.124898);
v.push_back(-2.027807);
v.push_back(0.132088);
v.push_back(1.108938);
v.push_back(-1.636803);
v.push_back(0.139563);
v.push_back(1.145777);
v.push_back(-1.721164);
v.push_back(0.229428);
v.push_back(1.140427);
v.push_back(-1.950117);
v.push_back(0.221293);
v.push_back(1.137827);
v.push_back(-1.595819);
v.push_back(0.158371);
v.push_back(1.159541);
v.push_back(-2.207737);
v.push_back(0.137477);
v.push_back(1.178530);
v.push_back(-1.448650);
v.push_back(0.457225);
v.push_back(1.178763);
v.push_back(-1.658702);
v.push_back(0.382985);
v.push_back(1.163302);
v.push_back(-1.939977);
v.push_back(0.345214);
v.push_back(1.138888);
v.push_back(-2.221144);
v.push_back(0.326565);
v.push_back(1.148615);
v.push_back(-2.296562);
v.push_back(0.277177);
v.push_back(1.150389);
v.push_back(-2.338212);
v.push_back(0.276654);
v.push_back(1.164009);
v.push_back(-2.334583);
v.push_back(0.282177);
v.push_back(1.169983);
v.push_back(-2.300825);
v.push_back(0.285040);
v.push_back(0.115987);
v.push_back(-2.500039);
v.push_back(0.354093);
v.push_back(0.073012);
v.push_back(-2.504526);
v.push_back(0.281053);
v.push_back(0.094479);
v.push_back(-2.497735);
v.push_back(0.366607);
v.push_back(0.051504);
v.push_back(-2.502222);
v.push_back(0.293566);
v.push_back(0.099970);
v.push_back(-2.394479);
v.push_back(0.357032);
v.push_back(0.056995);
v.push_back(-2.398967);
v.push_back(0.283992);
v.push_back(0.121478);
v.push_back(-2.396783);
v.push_back(0.344519);
v.push_back(0.078503);
v.push_back(-2.401271);
v.push_back(0.271479);
v.push_back(0.077555);
v.push_back(-2.463450);
v.push_back(0.340668);
v.push_back(0.064035);
v.push_back(-2.464862);
v.push_back(0.317689);
v.push_back(0.065762);
v.push_back(-2.432377);
v.push_back(0.314677);
v.push_back(0.078856);
v.push_back(-2.431142);
v.push_back(0.337087);
v.push_back(-0.049122);
v.push_back(-2.448942);
v.push_back(0.417889);
v.push_back(-0.061407);
v.push_back(-2.438572);
v.push_back(0.376480);
v.push_back(-0.050188);
v.push_back(-2.408185);
v.push_back(0.380735);
v.push_back(-0.037903);
v.push_back(-2.418555);
v.push_back(0.422143);
v.push_back(-0.092952);
v.push_back(-2.435871);
v.push_back(0.437190);
v.push_back(-0.083872);
v.push_back(-2.421200);
v.push_back(0.355145);
v.push_back(-0.071878);
v.push_back(-2.391250);
v.push_back(0.360288);
v.push_back(-0.080958);
v.push_back(-2.405921);
v.push_back(0.442334);
v.push_back(-0.175530);
v.push_back(-2.403933);
v.push_back(0.443780);
v.push_back(-0.166450);
v.push_back(-2.389262);
v.push_back(0.361735);
v.push_back(-0.154456);
v.push_back(-2.359312);
v.push_back(0.366879);
v.push_back(-0.163535);
v.push_back(-2.373983);
v.push_back(0.448924);
v.push_back(-0.054367);
v.push_back(-2.418975);
v.push_back(0.417225);
v.push_back(-0.064525);
v.push_back(-2.410149);
v.push_back(0.381236);
v.push_back(-0.087634);
v.push_back(-2.409209);
v.push_back(0.432800);
v.push_back(-0.080603);
v.push_back(-2.397848);
v.push_back(0.369263);
v.push_back(-0.151509);
v.push_back(-2.384495);
v.push_back(0.437843);
v.push_back(-0.144477);
v.push_back(-2.373134);
v.push_back(0.374306);
v.push_back(0.100508);
v.push_back(-2.398973);
v.push_back(0.308878);
v.push_back(0.079000);
v.push_back(-2.396669);
v.push_back(0.321391);
v.push_back(0.072259);
v.push_back(-2.431831);
v.push_back(0.325874);
v.push_back(-0.041071);
v.push_back(-2.414840);
v.push_back(0.407686);
v.push_back(-0.056498);
v.push_back(-2.416013);
v.push_back(0.405413);
v.push_back(-0.084203);
v.push_back(-2.403666);
v.push_back(0.401796);
v.push_back(-0.181772);
v.push_back(-2.377920);
v.push_back(0.403147);
v.push_back(-0.192800);
v.push_back(-2.365792);
v.push_back(0.405197);
v.push_back(-0.194142);
v.push_back(-2.413212);
v.push_back(0.393210);
v.push_back(-0.095555);
v.push_back(-2.452221);
v.push_back(0.391740);
v.push_back(-0.053723);
v.push_back(-2.445602);
v.push_back(0.406167);
v.push_back(0.070958);
v.push_back(-2.464139);
v.push_back(0.329455);
v.push_back(0.073509);
v.push_back(-2.499924);
v.push_back(0.330966);
v.push_back(0.095017);
v.push_back(-2.502228);
v.push_back(0.318452);
v.push_back(-0.048547);
v.push_back(-2.437293);
v.push_back(0.406516);
v.push_back(-0.050933);
v.push_back(-2.435561);
v.push_back(0.400436);
v.push_back(-0.045388);
v.push_back(-2.420541);
v.push_back(0.402539);
v.push_back(-0.043019);
v.push_back(-2.422270);
v.push_back(0.408588);
v.push_back(-0.040745);
v.push_back(-2.423921);
v.push_back(0.414382);
v.push_back(-0.046272);
v.push_back(-2.438944);
v.push_back(0.412310);
v.push_back(0.012618);
v.push_back(-2.451412);
v.push_back(0.364448);
v.push_back(0.016157);
v.push_back(-2.427213);
v.push_back(0.363862);
v.push_back(0.020695);
v.push_back(-2.427746);
v.push_back(0.372617);
v.push_back(0.025242);
v.push_back(-2.428167);
v.push_back(0.381259);
v.push_back(0.021936);
v.push_back(-2.452275);
v.push_back(0.382158);
v.push_back(0.017389);
v.push_back(-2.451854);
v.push_back(0.373516);
v.push_back(0.619206);
v.push_back(-2.070036);
v.push_back(0.394784);
v.push_back(0.275378);
v.push_back(-2.104731);
v.push_back(0.377963);
v.push_back(0.274673);
v.push_back(-2.058923);
v.push_back(0.292394);
v.push_back(0.619206);
v.push_back(-2.027111);
v.push_back(0.301319);
v.push_back(0.217165);
v.push_back(-2.360477);
v.push_back(0.175202);
v.push_back(0.213108);
v.push_back(-2.322452);
v.push_back(0.064735);
v.push_back(0.206968);
v.push_back(-1.295589);
v.push_back(0.026867);
v.push_back(0.619206);
v.push_back(-1.295589);
v.push_back(0.026867);
v.push_back(0.206968);
v.push_back(-1.276245);
v.push_back(0.203775);
v.push_back(0.619206);
v.push_back(-1.276245);
v.push_back(0.203775);
v.push_back(0.206968);
v.push_back(-1.248391);
v.push_back(0.237200);
v.push_back(0.619206);
v.push_back(-1.248391);
v.push_back(0.237200);
v.push_back(0.206968);
v.push_back(-1.120839);
v.push_back(0.240589);
v.push_back(0.619206);
v.push_back(-1.120839);
v.push_back(0.240589);
v.push_back(0.206968);
v.push_back(-1.095371);
v.push_back(0.229539);
v.push_back(0.619206);
v.push_back(-1.095371);
v.push_back(0.229539);
v.push_back(0.206968);
v.push_back(-1.108588);
v.push_back(0.249133);
v.push_back(0.619206);
v.push_back(-1.108588);
v.push_back(0.249133);
v.push_back(0.206968);
v.push_back(-1.050900);
v.push_back(0.428777);
v.push_back(0.619206);
v.push_back(-1.050900);
v.push_back(0.428777);
v.push_back(0.206968);
v.push_back(-1.021838);
v.push_back(0.442046);
v.push_back(0.619206);
v.push_back(-1.021838);
v.push_back(0.442046);
v.push_back(0.206968);
v.push_back(-1.032221);
v.push_back(0.476223);
v.push_back(0.619206);
v.push_back(-1.032221);
v.push_back(0.476223);
v.push_back(0.206968);
v.push_back(-0.988531);
v.push_back(0.545291);
v.push_back(0.619206);
v.push_back(-0.988531);
v.push_back(0.545291);
v.push_back(0.206968);
v.push_back(-0.924229);
v.push_back(0.533246);
v.push_back(0.619206);
v.push_back(-0.924229);
v.push_back(0.537420);
v.push_back(0.206968);
v.push_back(-0.925243);
v.push_back(0.412112);
v.push_back(0.619206);
v.push_back(-0.925243);
v.push_back(0.412112);
v.push_back(0.240456);
v.push_back(-0.784032);
v.push_back(0.482265);
v.push_back(0.619206);
v.push_back(-0.789251);
v.push_back(0.498544);
v.push_back(0.277397);
v.push_back(-0.798366);
v.push_back(0.538827);
v.push_back(0.322376);
v.push_back(-1.131551);
v.push_back(0.629017);
v.push_back(0.619206);
v.push_back(-1.223619);
v.push_back(0.629017);
v.push_back(0.302953);
v.push_back(-1.516591);
v.push_back(0.667885);
v.push_back(0.619206);
v.push_back(-1.509198);
v.push_back(0.681925);
v.push_back(0.203416);
v.push_back(-2.371559);
v.push_back(0.363557);
v.push_back(0.619206);
v.push_back(-2.554899);
v.push_back(0.453453);
v.push_back(0.619206);
v.push_back(-2.339426);
v.push_back(-0.002046);
v.push_back(0.097477);
v.push_back(-2.394618);
v.push_back(-0.015811);
v.push_back(0.619206);
v.push_back(-2.480318);
v.push_back(0.247874);
v.push_back(0.196257);
v.push_back(-2.645944);
v.push_back(0.195204);
v.push_back(0.619206);
v.push_back(-3.154164);
v.push_back(0.281493);
v.push_back(0.205932);
v.push_back(-3.029875);
v.push_back(0.281485);
v.push_back(0.619206);
v.push_back(-3.408658);
v.push_back(0.210661);
v.push_back(0.206725);
v.push_back(-3.268776);
v.push_back(0.210659);
v.push_back(0.619206);
v.push_back(-3.540860);
v.push_back(-0.007156);
v.push_back(0.137517);
v.push_back(-3.419832);
v.push_back(-0.007156);
v.push_back(0.619206);
v.push_back(-3.747685);
v.push_back(-0.007156);
v.push_back(0.180269);
v.push_back(-3.611537);
v.push_back(-0.007156);
v.push_back(0.619206);
v.push_back(-3.753917);
v.push_back(0.105561);
v.push_back(0.180269);
v.push_back(-3.614132);
v.push_back(0.105561);
v.push_back(0.619206);
v.push_back(-3.637131);
v.push_back(0.242580);
v.push_back(0.206968);
v.push_back(-3.478498);
v.push_back(0.233931);
v.push_back(0.619206);
v.push_back(-3.251727);
v.push_back(0.425073);
v.push_back(0.203804);
v.push_back(-3.164988);
v.push_back(0.367427);
v.push_back(0.619206);
v.push_back(-2.938191);
v.push_back(0.493586);
v.push_back(0.193476);
v.push_back(-2.909405);
v.push_back(0.445384);
v.push_back(0.619206);
v.push_back(-2.560987);
v.push_back(0.481919);
v.push_back(0.194660);
v.push_back(-2.370386);
v.push_back(0.393172);
v.push_back(0.619206);
v.push_back(-1.518557);
v.push_back(0.717163);
v.push_back(0.302953);
v.push_back(-1.525950);
v.push_back(0.703123);
v.push_back(0.619206);
v.push_back(-1.182667);
v.push_back(0.707745);
v.push_back(0.322376);
v.push_back(-1.114701);
v.push_back(0.659034);
v.push_back(0.619206);
v.push_back(-0.700763);
v.push_back(0.555536);
v.push_back(0.165245);
v.push_back(-0.704083);
v.push_back(0.549493);
v.push_back(0.619206);
v.push_back(-0.240809);
v.push_back(0.513990);
v.push_back(0.091620);
v.push_back(-0.282661);
v.push_back(0.511429);
v.push_back(0.619206);
v.push_back(-0.091269);
v.push_back(0.416976);
v.push_back(0.119846);
v.push_back(-0.090609);
v.push_back(0.378523);
v.push_back(0.619206);
v.push_back(-0.002719);
v.push_back(0.273364);
v.push_back(0.210284);
v.push_back(-0.017588);
v.push_back(0.256014);
v.push_back(0.619206);
v.push_back(-0.027053);
v.push_back(0.142304);
v.push_back(0.619206);
v.push_back(-0.167726);
v.push_back(0.007059);
v.push_back(0.619206);
v.push_back(-0.191891);
v.push_back(0.193277);
v.push_back(-0.064039);
v.push_back(-0.272725);
v.push_back(0.212854);
v.push_back(0.619206);
v.push_back(-0.664286);
v.push_back(0.236738);
v.push_back(-0.126060);
v.push_back(-0.742655);
v.push_back(0.372432);
v.push_back(0.619206);
v.push_back(-0.788646);
v.push_back(0.195285);
v.push_back(-0.077531);
v.push_back(-0.935045);
v.push_back(0.128320);
v.push_back(0.619206);
v.push_back(-1.136863);
v.push_back(0.161395);
v.push_back(0.091802);
v.push_back(-1.023064);
v.push_back(-0.046048);
v.push_back(-0.133148);
v.push_back(-2.612969);
v.push_back(0.253162);
v.push_back(-0.104416);
v.push_back(-2.568455);
v.push_back(-0.003469);
v.push_back(0.032888);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(0.043118);
v.push_back(-2.325799);
v.push_back(0.210149);
v.push_back(-0.078749);
v.push_back(-2.987251);
v.push_back(0.490857);
v.push_back(-0.108021);
v.push_back(-3.140682);
v.push_back(0.432154);
v.push_back(-0.111469);
v.push_back(-3.290463);
v.push_back(0.254919);
v.push_back(-0.033221);
v.push_back(-3.418578);
v.push_back(0.104449);
v.push_back(-0.024446);
v.push_back(-3.393188);
v.push_back(-0.007156);
v.push_back(-0.095099);
v.push_back(-3.185471);
v.push_back(-0.007156);
v.push_back(-0.118693);
v.push_back(-3.091060);
v.push_back(0.337217);
v.push_back(-0.132080);
v.push_back(-2.965753);
v.push_back(0.411659);
v.push_back(-0.054951);
v.push_back(-1.079122);
v.push_back(-0.030699);
v.push_back(-0.020387);
v.push_back(-0.308943);
v.push_back(0.491000);
v.push_back(-0.023722);
v.push_back(-0.133172);
v.push_back(0.341560);
v.push_back(-0.021490);
v.push_back(-0.778115);
v.push_back(0.492824);
v.push_back(0.111777);
v.push_back(-0.240992);
v.push_back(0.089374);
v.push_back(0.109314);
v.push_back(-0.073357);
v.push_back(0.117219);
v.push_back(0.019994);
v.push_back(-0.074698);
v.push_back(0.207510);
v.push_back(0.206968);
v.push_back(-1.037141);
v.push_back(0.195446);
v.push_back(-0.009179);
v.push_back(-1.063455);
v.push_back(0.204596);
v.push_back(-0.039540);
v.push_back(-0.982025);
v.push_back(0.386366);
v.push_back(0.206968);
v.push_back(-0.960873);
v.push_back(0.349901);
v.push_back(-0.132356);
v.push_back(-0.969725);
v.push_back(0.173725);
v.push_back(-0.139535);
v.push_back(-0.749190);
v.push_back(0.386768);
v.push_back(-0.129144);
v.push_back(-0.333755);
v.push_back(0.375961);
v.push_back(-0.086326);
v.push_back(-0.244227);
v.push_back(0.255415);
v.push_back(0.021396);
v.push_back(-0.218379);
v.push_back(0.152738);
v.push_back(0.619206);
v.push_back(-0.539833);
v.push_back(0.543025);
v.push_back(0.173895);
v.push_back(-0.529311);
v.push_back(0.529713);
v.push_back(-0.001064);
v.push_back(-0.549862);
v.push_back(0.529236);
v.push_back(-0.145200);
v.push_back(-0.532760);
v.push_back(0.426001);
v.push_back(-0.123126);
v.push_back(-0.534232);
v.push_back(0.415145);
v.push_back(0.619206);
v.push_back(-0.470452);
v.push_back(0.234775);
v.push_back(0.007102);
v.push_back(-2.021386);
v.push_back(-0.002551);
v.push_back(0.027864);
v.push_back(-2.036724);
v.push_back(0.071738);
v.push_back(0.214347);
v.push_back(-2.040543);
v.push_back(0.018778);
v.push_back(0.211844);
v.push_back(-2.026747);
v.push_back(-0.004033);
v.push_back(0.619206);
v.push_back(-2.014948);
v.push_back(-0.002046);
v.push_back(0.619206);
v.push_back(-1.388983);
v.push_back(0.664098);
v.push_back(0.251732);
v.push_back(-1.391378);
v.push_back(0.651580);
v.push_back(0.241957);
v.push_back(-1.387985);
v.push_back(0.683317);
v.push_back(0.619206);
v.push_back(-1.385237);
v.push_back(0.717933);
v.push_back(0.619206);
v.push_back(-1.516238);
v.push_back(-0.002046);
v.push_back(0.206968);
v.push_back(-1.518217);
v.push_back(-0.002046);
v.push_back(0.042033);
v.push_back(-1.625082);
v.push_back(-0.003536);
v.push_back(0.042192);
v.push_back(-1.626360);
v.push_back(0.076337);
v.push_back(0.206968);
v.push_back(-1.498765);
v.push_back(0.026867);
v.push_back(0.619206);
v.push_back(-1.494381);
v.push_back(0.026867);
v.push_back(0.086506);
v.push_back(-1.037141);
v.push_back(0.195446);
v.push_back(0.102310);
v.push_back(-1.423867);
v.push_back(0.077824);
v.push_back(0.116217);
v.push_back(-1.621897);
v.push_back(0.074964);
v.push_back(0.088395);
v.push_back(-2.042560);
v.push_back(0.069819);
v.push_back(0.081756);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(0.080210);
v.push_back(-2.323577);
v.push_back(0.209123);
v.push_back(0.082335);
v.push_back(-1.390671);
v.push_back(0.503985);
v.push_back(0.089539);
v.push_back(-1.276115);
v.push_back(0.555584);
v.push_back(0.046284);
v.push_back(-1.274665);
v.push_back(0.565373);
v.push_back(0.034355);
v.push_back(-1.396198);
v.push_back(0.514830);
v.push_back(0.102858);
v.push_back(-1.043489);
v.push_back(0.556955);
v.push_back(0.130498);
v.push_back(-2.351243);
v.push_back(0.332705);
v.push_back(0.108058);
v.push_back(-2.361173);
v.push_back(0.350132);
v.push_back(0.084191);
v.push_back(-2.939534);
v.push_back(0.498342);
v.push_back(0.100255);
v.push_back(-3.152961);
v.push_back(0.435687);
v.push_back(0.016480);
v.push_back(-3.427473);
v.push_back(0.266761);
v.push_back(0.042581);
v.push_back(-3.546065);
v.push_back(0.104449);
v.push_back(0.070109);
v.push_back(-3.537933);
v.push_back(-0.007156);
v.push_back(0.253433);
v.push_back(-1.491513);
v.push_back(0.650230);
v.push_back(0.164723);
v.push_back(-1.435365);
v.push_back(0.594151);
v.push_back(0.160695);
v.push_back(-1.331443);
v.push_back(0.594945);
v.push_back(0.141459);
v.push_back(-1.327967);
v.push_back(0.629858);
v.push_back(0.142201);
v.push_back(-1.441454);
v.push_back(0.631052);
v.push_back(0.242689);
v.push_back(-1.499806);
v.push_back(0.684696);
v.push_back(0.275156);
v.push_back(-1.710163);
v.push_back(0.645088);
v.push_back(0.223334);
v.push_back(-1.706746);
v.push_back(0.646460);
v.push_back(0.215437);
v.push_back(-1.721245);
v.push_back(0.674570);
v.push_back(0.275625);
v.push_back(-1.718123);
v.push_back(0.690895);
v.push_back(0.225741);
v.push_back(-2.375983);
v.push_back(0.342015);
v.push_back(0.365206);
v.push_back(-2.475131);
v.push_back(0.411308);
v.push_back(0.365962);
v.push_back(-2.479384);
v.push_back(0.435649);
v.push_back(0.366916);
v.push_back(-2.903164);
v.push_back(0.493586);
v.push_back(0.293001);
v.push_back(-3.173718);
v.push_back(0.431019);
v.push_back(0.249741);
v.push_back(-3.493051);
v.push_back(0.248852);
v.push_back(0.240321);
v.push_back(-3.609424);
v.push_back(0.131368);
v.push_back(0.244018);
v.push_back(-3.624381);
v.push_back(-0.007156);
v.push_back(0.281262);
v.push_back(-0.770148);
v.push_back(0.561802);
v.push_back(0.365976);
v.push_back(-1.073111);
v.push_back(0.648620);
v.push_back(0.619206);
v.push_back(-0.770066);
v.push_back(0.563511);
v.push_back(0.619206);
v.push_back(-1.103696);
v.push_back(0.648043);
v.push_back(0.368341);
v.push_back(-1.071552);
v.push_back(0.620301);
v.push_back(0.619206);
v.push_back(-1.116080);
v.push_back(0.620071);
v.push_back(0.619206);
v.push_back(-0.801650);
v.push_back(0.547744);
v.push_back(0.211605);
v.push_back(-0.813962);
v.push_back(0.552109);
v.push_back(0.212453);
v.push_back(-0.838472);
v.push_back(0.538828);
v.push_back(0.072623);
v.push_back(-1.043899);
v.push_back(0.574793);
v.push_back(0.409897);
v.push_back(-1.390224);
v.push_back(0.657614);
v.push_back(0.334694);
v.push_back(-1.389088);
v.push_back(0.710988);
v.push_back(0.404456);
v.push_back(-1.178303);
v.push_back(0.701253);
v.push_back(0.469073);
v.push_back(-1.103696);
v.push_back(0.648043);
v.push_back(0.470298);
v.push_back(-1.108858);
v.push_back(0.619892);
v.push_back(0.446489);
v.push_back(-1.175929);
v.push_back(0.629017);
v.push_back(0.243029);
v.push_back(-1.840608);
v.push_back(0.635255);
v.push_back(0.209435);
v.push_back(-1.834074);
v.push_back(0.618363);
v.push_back(0.243352);
v.push_back(-1.855102);
v.push_back(0.661676);
v.push_back(0.204356);
v.push_back(-1.844445);
v.push_back(0.646170);
v.push_back(0.619206);
v.push_back(-1.719647);
v.push_back(0.688595);
v.push_back(0.619206);
v.push_back(-1.850204);
v.push_back(0.668273);
v.push_back(0.619206);
v.push_back(-1.864695);
v.push_back(0.694704);
v.push_back(0.619206);
v.push_back(-1.727607);
v.push_back(0.719811);
v.push_back(0.426528);
v.push_back(-1.865862);
v.push_back(0.686269);
v.push_back(0.431034);
v.push_back(-1.728753);
v.push_back(0.711316);
v.push_back(0.430820);
v.push_back(-1.720793);
v.push_back(0.680101);
v.push_back(0.426380);
v.push_back(-1.851370);
v.push_back(0.659843);
v.push_back(0.032620);
v.push_back(-0.986319);
v.push_back(0.383949);
v.push_back(0.029915);
v.push_back(-1.068791);
v.push_back(0.270624);
v.push_back(0.114465);
v.push_back(-0.786802);
v.push_back(0.480514);
v.push_back(0.015601);
v.push_back(-1.144979);
v.push_back(0.440152);
v.push_back(0.004197);
v.push_back(-1.456803);
v.push_back(0.257471);
v.push_back(0.062981);
v.push_back(-1.453670);
v.push_back(0.254944);
v.push_back(0.619206);
v.push_back(-1.365559);
v.push_back(-0.002046);
v.push_back(0.185382);
v.push_back(-1.270085);
v.push_back(-0.046434);
v.push_back(0.006316);
v.push_back(-1.236264);
v.push_back(-0.026937);
v.push_back(0.051574);
v.push_back(-1.139792);
v.push_back(0.077133);
v.push_back(0.042807);
v.push_back(-1.563865);
v.push_back(0.123649);
v.push_back(0.106845);
v.push_back(-1.562341);
v.push_back(0.123414);
v.push_back(0.080719);
v.push_back(-1.231317);
v.push_back(0.115898);
v.push_back(0.206968);
v.push_back(-1.243803);
v.push_back(0.114517);
v.push_back(0.137251);
v.push_back(-1.032894);
v.push_back(0.471973);
v.push_back(0.619206);
v.push_back(-2.776860);
v.push_back(0.498546);
v.push_back(0.352670);
v.push_back(-2.776860);
v.push_back(0.498546);
v.push_back(0.189174);
v.push_back(-2.654077);
v.push_back(0.437150);
v.push_back(0.114884);
v.push_back(-2.695718);
v.push_back(0.409832);
v.push_back(-0.071761);
v.push_back(-2.721582);
v.push_back(0.448029);
v.push_back(-0.148729);
v.push_back(-2.751805);
v.push_back(0.377337);
v.push_back(0.190519);
v.push_back(-2.808951);
v.push_back(0.258063);
v.push_back(0.619206);
v.push_back(-2.873657);
v.push_back(0.258188);
v.push_back(-0.114747);
v.push_back(-0.365493);
v.push_back(0.355766);
v.push_back(0.619206);
v.push_back(-0.316433);
v.push_back(0.215018);
v.push_back(0.001069);
v.push_back(-2.354544);
v.push_back(-0.009461);
v.push_back(0.009186);
v.push_back(-2.492751);
v.push_back(0.075198);
v.push_back(0.026095);
v.push_back(-2.499173);
v.push_back(0.232726);
v.push_back(0.619206);
v.push_back(-0.101984);
v.push_back(0.021427);
v.push_back(0.107425);
v.push_back(-0.163595);
v.push_back(0.095729);
v.push_back(0.019846);
v.push_back(-0.172423);
v.push_back(0.172763);
v.push_back(-0.085539);
v.push_back(-0.205694);
v.push_back(0.310199);
v.push_back(-0.124683);
v.push_back(-0.317187);
v.push_back(0.447141);
v.push_back(-0.132422);
v.push_back(-0.532404);
v.push_back(0.494807);
v.push_back(-0.118494);
v.push_back(-0.753248);
v.push_back(0.455066);
v.push_back(-0.138475);
v.push_back(-0.999171);
v.push_back(0.292138);
v.push_back(-0.088454);
v.push_back(-1.109970);
v.push_back(0.114258);
v.push_back(-0.000540);
v.push_back(-1.202736);
v.push_back(0.065268);
v.push_back(0.032929);
v.push_back(-1.625497);
v.push_back(0.056741);
v.push_back(0.007562);
v.push_back(-2.026692);
v.push_back(0.057845);
v.push_back(0.009216);
v.push_back(-2.313357);
v.push_back(0.049964);
v.push_back(-0.069174);
v.push_back(-2.501458);
v.push_back(0.078989);
v.push_back(-0.083477);
v.push_back(-2.527011);
v.push_back(0.253414);
v.push_back(0.329006);
v.push_back(-0.014614);
v.push_back(0.131710);
v.push_back(0.327082);
v.push_back(-0.136548);
v.push_back(0.009332);
v.push_back(0.328847);
v.push_back(-0.209913);
v.push_back(-0.006744);
v.push_back(0.265653);
v.push_back(-0.257127);
v.push_back(0.192156);
v.push_back(0.256634);
v.push_back(-0.367784);
v.push_back(0.222208);
v.push_back(0.265078);
v.push_back(-0.495640);
v.push_back(0.240044);
v.push_back(0.258365);
v.push_back(-0.670942);
v.push_back(0.251646);
v.push_back(0.241937);
v.push_back(-0.802965);
v.push_back(0.215419);
v.push_back(0.320463);
v.push_back(-1.106456);
v.push_back(0.155693);
v.push_back(0.381588);
v.push_back(-1.374654);
v.push_back(-0.002046);
v.push_back(0.381588);
v.push_back(-1.517290);
v.push_back(-0.002046);
v.push_back(0.381588);
v.push_back(-2.017052);
v.push_back(-0.002046);
v.push_back(0.332297);
v.push_back(-2.359802);
v.push_back(-0.002507);
v.push_back(0.375486);
v.push_back(-2.442568);
v.push_back(0.187990);
v.push_back(0.381588);
v.push_back(-2.841912);
v.push_back(0.258188);
v.push_back(0.381588);
v.push_back(-3.117023);
v.push_back(0.281493);
v.push_back(0.381588);
v.push_back(-3.367006);
v.push_back(0.210661);
v.push_back(0.250227);
v.push_back(-3.485275);
v.push_back(-0.007156);
v.push_back(-0.008592);
v.push_back(-3.194801);
v.push_back(-0.007156);
v.push_back(0.030399);
v.push_back(-3.131553);
v.push_back(0.279635);
v.push_back(-0.007731);
v.push_back(-2.965584);
v.push_back(0.360348);
v.push_back(-0.014237);
v.push_back(-2.776562);
v.push_back(0.327614);
v.push_back(-0.004434);
v.push_back(-2.627140);
v.push_back(0.229537);
v.push_back(0.013241);
v.push_back(-2.568015);
v.push_back(-0.003445);
v.push_back(0.619206);
v.push_back(-0.005220);
v.push_back(0.184852);
v.push_back(0.285536);
v.push_back(-0.027482);
v.push_back(0.252290);
v.push_back(0.376335);
v.push_back(-0.013018);
v.push_back(0.184080);
v.push_back(0.202316);
v.push_back(-0.112550);
v.push_back(0.362862);
v.push_back(0.186315);
v.push_back(-0.290281);
v.push_back(0.481142);
v.push_back(0.282868);
v.push_back(-0.513419);
v.push_back(0.508797);
v.push_back(0.619206);
v.push_back(-0.638935);
v.push_back(0.516193);
v.push_back(0.379043);
v.push_back(-0.615700);
v.push_back(0.512753);
v.push_back(-0.120082);
v.push_back(-2.521896);
v.push_back(0.064375);
v.push_back(-0.138951);
v.push_back(-2.553540);
v.push_back(0.262590);
v.push_back(-0.138843);
v.push_back(-2.726707);
v.push_back(0.432466);
v.push_back(-0.138305);
v.push_back(-2.975591);
v.push_back(0.466490);
v.push_back(-0.148448);
v.push_back(-3.115956);
v.push_back(0.389399);
v.push_back(-0.138253);
v.push_back(-3.242014);
v.push_back(0.114670);
v.push_back(-0.041282);
v.push_back(-3.410922);
v.push_back(0.045148);
v.push_back(0.047514);
v.push_back(-3.551581);
v.push_back(0.044256);
v.push_back(0.176598);
v.push_back(-3.626053);
v.push_back(0.047722);
v.push_back(0.240193);
v.push_back(-3.630784);
v.push_back(0.059451);
v.push_back(0.619206);
v.push_back(-3.763515);
v.push_back(0.047702);
v.push_back(0.144743);
v.push_back(-3.441600);
v.push_back(0.281635);
v.push_back(0.206708);
v.push_back(-3.441600);
v.push_back(0.281635);
v.push_back(0.231553);
v.push_back(-3.428584);
v.push_back(0.223936);
v.push_back(0.206708);
v.push_back(-3.441600);
v.push_back(0.166237);
v.push_back(0.144743);
v.push_back(-3.441600);
v.push_back(0.166237);
v.push_back(0.119898);
v.push_back(-3.428584);
v.push_back(0.223936);
v.push_back(0.148968);
v.push_back(-3.559946);
v.push_back(0.270282);
v.push_back(0.202483);
v.push_back(-3.559946);
v.push_back(0.270282);
v.push_back(0.229241);
v.push_back(-3.559946);
v.push_back(0.223936);
v.push_back(0.202483);
v.push_back(-3.559946);
v.push_back(0.177590);
v.push_back(0.148968);
v.push_back(-3.559946);
v.push_back(0.177590);
v.push_back(0.122210);
v.push_back(-3.559946);
v.push_back(0.223936);
v.push_back(0.159657);
v.push_back(-3.585260);
v.push_back(0.253582);
v.push_back(0.191794);
v.push_back(-3.585260);
v.push_back(0.253582);
v.push_back(0.203231);
v.push_back(-3.587827);
v.push_back(0.223936);
v.push_back(0.191794);
v.push_back(-3.585260);
v.push_back(0.194289);
v.push_back(0.159657);
v.push_back(-3.585260);
v.push_back(0.194289);
v.push_back(0.148220);
v.push_back(-3.587827);
v.push_back(0.223936);
v.push_back(0.143004);
v.push_back(-3.568291);
v.push_back(0.280612);
v.push_back(0.208447);
v.push_back(-3.568291);
v.push_back(0.280612);
v.push_back(0.241169);
v.push_back(-3.568291);
v.push_back(0.223936);
v.push_back(0.208447);
v.push_back(-3.568291);
v.push_back(0.167260);
v.push_back(0.143004);
v.push_back(-3.568291);
v.push_back(0.167260);
v.push_back(0.110282);
v.push_back(-3.568291);
v.push_back(0.223936);
v.push_back(0.090184);
v.push_back(-0.351157);
v.push_back(0.518271);
v.push_back(0.044287);
v.push_back(-0.351157);
v.push_back(0.518271);
v.push_back(0.025885);
v.push_back(-0.433569);
v.push_back(0.475535);
v.push_back(0.044287);
v.push_back(-0.413691);
v.push_back(0.432798);
v.push_back(0.090184);
v.push_back(-0.413691);
v.push_back(0.432798);
v.push_back(0.108586);
v.push_back(-0.433569);
v.push_back(0.475535);
v.push_back(0.087055);
v.push_back(-0.155932);
v.push_back(0.509862);
v.push_back(0.047416);
v.push_back(-0.155932);
v.push_back(0.509862);
v.push_back(0.027597);
v.push_back(-0.155932);
v.push_back(0.475535);
v.push_back(0.047416);
v.push_back(-0.155932);
v.push_back(0.441207);
v.push_back(0.087055);
v.push_back(-0.155932);
v.push_back(0.441207);
v.push_back(0.106874);
v.push_back(-0.155932);
v.push_back(0.475535);
v.push_back(0.079137);
v.push_back(-0.131588);
v.push_back(0.497493);
v.push_back(0.055334);
v.push_back(-0.131588);
v.push_back(0.497493);
v.push_back(0.046863);
v.push_back(-0.129686);
v.push_back(0.475535);
v.push_back(0.055334);
v.push_back(-0.131588);
v.push_back(0.453576);
v.push_back(0.079137);
v.push_back(-0.131588);
v.push_back(0.453576);
v.push_back(0.087609);
v.push_back(-0.129686);
v.push_back(0.475535);
v.push_back(0.091472);
v.push_back(-0.144156);
v.push_back(0.517514);
v.push_back(0.097252);
v.push_back(-0.146807);
v.push_back(0.527433);
v.push_back(0.042999);
v.push_back(-0.144156);
v.push_back(0.517514);
v.push_back(0.037219);
v.push_back(-0.146807);
v.push_back(0.527433);
v.push_back(0.018762);
v.push_back(-0.144156);
v.push_back(0.475535);
v.push_back(0.007268);
v.push_back(-0.149775);
v.push_back(0.475482);
v.push_back(0.042999);
v.push_back(-0.144156);
v.push_back(0.433556);
v.push_back(0.037255);
v.push_back(-0.150298);
v.push_back(0.423593);
v.push_back(0.091472);
v.push_back(-0.144156);
v.push_back(0.433556);
v.push_back(0.097216);
v.push_back(-0.150298);
v.push_back(0.423593);
v.push_back(0.115709);
v.push_back(-0.144156);
v.push_back(0.475535);
v.push_back(0.127203);
v.push_back(-0.149775);
v.push_back(0.475482);
v.push_back(0.203691);
v.push_back(-0.823492);
v.push_back(0.478551);
v.push_back(0.139422);
v.push_back(-0.851288);
v.push_back(0.541428);
v.push_back(0.154636);
v.push_back(-0.863615);
v.push_back(0.525919);
v.push_back(0.183392);
v.push_back(-0.930858);
v.push_back(0.472690);
v.push_back(0.102009);
v.push_back(-0.807001);
v.push_back(0.524088);
v.push_back(0.030156);
v.push_back(-0.996463);
v.push_back(0.489120);
v.push_back(0.076377);
v.push_back(-1.569560);
v.push_back(0.127032);
v.push_back(0.035276);
v.push_back(-1.461312);
v.push_back(0.260222);
v.push_back(0.059378);
v.push_back(-1.400697);
v.push_back(0.508775);
v.push_back(0.152350);
v.push_back(-1.445064);
v.push_back(0.609400);
v.push_back(0.242282);
v.push_back(-1.498932);
v.push_back(0.664180);
v.push_back(0.212375);
v.push_back(-1.712729);
v.push_back(0.658493);
v.push_back(0.199750);
v.push_back(-1.838051);
v.push_back(0.630740);
v.push_back(0.115648);
v.push_back(-2.349854);
v.push_back(0.340978);
v.push_back(0.061883);
v.push_back(-2.317602);
v.push_back(0.211766);
v.push_back(0.058225);
v.push_back(-2.275416);
v.push_back(0.074770);
v.push_back(0.059423);
v.push_back(-2.039831);
v.push_back(0.078120);
v.push_back(0.080944);
v.push_back(-1.629162);
v.push_back(0.080931);
v.push_back(0.619206);
v.push_back(-1.801190);
v.push_back(0.720054);
v.push_back(0.427362);
v.push_back(-1.805232);
v.push_back(0.711006);
v.push_back(0.273806);
v.push_back(-1.793895);
v.push_back(0.686101);
v.push_back(0.224381);
v.push_back(-1.851597);
v.push_back(0.659349);
v.push_back(0.237494);
v.push_back(-1.789770);
v.push_back(0.675553);
v.push_back(0.152615);
v.push_back(-2.367141);
v.push_back(0.377764);
v.push_back(0.109028);
v.push_back(-2.408738);
v.push_back(0.358620);
v.push_back(0.143396);
v.push_back(-2.408587);
v.push_back(0.374867);
v.push_back(0.036091);
v.push_back(-2.397367);
v.push_back(0.219469);
v.push_back(0.019893);
v.push_back(-2.396524);
v.push_back(0.072304);
v.push_back(-0.046016);
v.push_back(-2.416159);
v.push_back(0.070414);
v.push_back(0.619206);
v.push_back(-3.524278);
v.push_back(0.107366);
v.push_back(0.323982);
v.push_back(-3.475739);
v.push_back(0.107366);
v.push_back(0.176512);
v.push_back(-3.389982);
v.push_back(0.107366);
v.push_back(0.019860);
v.push_back(-3.201705);
v.push_back(0.173858);
v.push_back(-0.106043);
v.push_back(-3.167883);
v.push_back(0.207267);
v.push_back(-0.145722);
v.push_back(-3.209247);
v.push_back(0.260070);
v.push_back(-0.114569);
v.push_back(-3.246625);
v.push_back(0.348742);
v.push_back(0.066281);
v.push_back(-3.323923);
v.push_back(0.355239);
v.push_back(0.206968);
v.push_back(-3.360304);
v.push_back(0.299884);
v.push_back(0.261314);
v.push_back(-3.374163);
v.push_back(0.344779);
v.push_back(0.619206);
v.push_back(-3.488541);
v.push_back(0.337667);
v.push_back(0.223618);
v.push_back(-2.046117);
v.push_back(0.551629);
v.push_back(0.212367);
v.push_back(-2.198211);
v.push_back(0.473295);
v.push_back(0.186343);
v.push_back(-2.048837);
v.push_back(0.544868);
v.push_back(0.162752);
v.push_back(-2.200930);
v.push_back(0.459107);
v.push_back(0.152801);
v.push_back(-2.201298);
v.push_back(0.468036);
v.push_back(0.177493);
v.push_back(-2.054041);
v.push_back(0.553074);
v.push_back(0.225171);
v.push_back(-2.060602);
v.push_back(0.572843);
v.push_back(0.210038);
v.push_back(-2.220006);
v.push_back(0.488746);
v.push_back(0.182338);
v.push_back(-2.064425);
v.push_back(0.564485);
v.push_back(0.154102);
v.push_back(-2.210785);
v.push_back(0.478392);
v.push_back(0.181291);
v.push_back(-2.216674);
v.push_back(0.488598);
v.push_back(0.202579);
v.push_back(-2.067473);
v.push_back(0.572011);
v.push_back(0.443911);
v.push_back(-2.082741);
v.push_back(0.386180);
v.push_back(0.508359);
v.push_back(-2.033090);
v.push_back(0.301180);
v.push_back(0.529936);
v.push_back(-2.323279);
v.push_back(0.196732);
v.push_back(0.561877);
v.push_back(-2.348285);
v.push_back(0.064735);
v.push_back(0.519080);
v.push_back(-1.779973);
v.push_back(0.026196);
v.push_back(0.540470);
v.push_back(-1.495218);
v.push_back(0.026867);
v.push_back(0.545792);
v.push_back(-2.069375);
v.push_back(0.283606);
v.push_back(0.619206);
v.push_back(-2.063396);
v.push_back(0.289322);
v.push_back(0.560575);
v.push_back(-1.963798);
v.push_back(0.151089);
v.push_back(0.619206);
v.push_back(-1.968045);
v.push_back(0.151089);
v.push_back(0.554584);
v.push_back(-1.718724);
v.push_back(0.076091);
v.push_back(0.619206);
v.push_back(-1.716246);
v.push_back(0.076642);
v.push_back(0.463849);
v.push_back(-1.625749);
v.push_back(0.026609);
v.push_back(0.247756);
v.push_back(-2.190158);
v.push_back(0.258782);
v.push_back(0.249556);
v.push_back(-2.255238);
v.push_back(0.356799);
v.push_back(0.433235);
v.push_back(-2.319337);
v.push_back(0.399678);
v.push_back(0.619206);
v.push_back(-2.353820);
v.push_back(0.431314);
v.push_back(0.619206);
v.push_back(-2.514182);
v.push_back(0.448970);
v.push_back(0.378982);
v.push_back(-2.443584);
v.push_back(0.408953);
v.push_back(0.195202);
v.push_back(-2.374978);
v.push_back(0.303964);
v.push_back(0.230105);
v.push_back(-2.222697);
v.push_back(0.324140);
v.push_back(0.243924);
v.push_back(-2.080982);
v.push_back(0.348864);
v.push_back(0.482799);
v.push_back(-2.043308);
v.push_back(0.356922);
v.push_back(0.619206);
v.push_back(-2.034240);
v.push_back(0.360948);
v.push_back(0.283890);
v.push_back(-0.708194);
v.push_back(0.544073);
v.push_back(0.253697);
v.push_back(-0.516008);
v.push_back(0.525980);
v.push_back(0.162382);
v.push_back(-0.284011);
v.push_back(0.500126);
v.push_back(0.181808);
v.push_back(-0.099126);
v.push_back(0.375862);
v.push_back(0.265572);
v.push_back(-0.014266);
v.push_back(0.258144);
v.push_back(0.362993);
v.push_back(-0.001950);
v.push_back(0.168466);
v.push_back(0.619206);
v.push_back(-0.000250);
v.push_back(0.169920);
v.push_back(0.032145);
v.push_back(-1.280079);
v.push_back(0.150962);
v.push_back(0.014682);
v.push_back(-1.340069);
v.push_back(0.265086);
v.push_back(0.034439);
v.push_back(-1.339459);
v.push_back(0.541293);
v.push_back(0.138216);
v.push_back(-1.388427);
v.push_back(0.635454);
v.push_back(0.240011);
v.push_back(-1.447172);
v.push_back(0.689750);
v.push_back(0.316878);
v.push_back(-1.461649);
v.push_back(0.712901);
v.push_back(0.619206);
v.push_back(-1.456040);
v.push_back(0.723681);
v.push_back(0.619206);
v.push_back(-1.256290);
v.push_back(0.112082);
v.push_back(0.095568);
v.push_back(-0.276497);
v.push_back(0.524608);
v.push_back(0.038903);
v.push_back(-0.276497);
v.push_back(0.524608);
v.push_back(0.010570);
v.push_back(-0.316698);
v.push_back(0.475535);
v.push_back(0.038903);
v.push_back(-0.323398);
v.push_back(0.426461);
v.push_back(0.095568);
v.push_back(-0.323398);
v.push_back(0.426461);
v.push_back(0.123901);
v.push_back(-0.316698);
v.push_back(0.475535);
v.push_back(0.083540);
v.push_back(-0.137021);
v.push_back(0.503775);
v.push_back(0.050931);
v.push_back(-0.137021);
v.push_back(0.503775);
v.push_back(0.034627);
v.push_back(-0.137021);
v.push_back(0.475535);
v.push_back(0.050931);
v.push_back(-0.137021);
v.push_back(0.447294);
v.push_back(0.083540);
v.push_back(-0.137021);
v.push_back(0.447294);
v.push_back(0.099844);
v.push_back(-0.137021);
v.push_back(0.475535);
v.push_back(0.137474);
v.push_back(-3.478788);
v.push_back(0.290190);
v.push_back(0.213977);
v.push_back(-3.478788);
v.push_back(0.290190);
v.push_back(0.252229);
v.push_back(-3.478788);
v.push_back(0.223936);
v.push_back(0.213977);
v.push_back(-3.478788);
v.push_back(0.157681);
v.push_back(0.137474);
v.push_back(-3.478788);
v.push_back(0.157681);
v.push_back(0.099222);
v.push_back(-3.478788);
v.push_back(0.223936);
v.push_back(0.153713);
v.push_back(-3.577924);
v.push_back(0.262063);
v.push_back(0.197738);
v.push_back(-3.577924);
v.push_back(0.262063);
v.push_back(0.219751);
v.push_back(-3.577924);
v.push_back(0.223936);
v.push_back(0.197738);
v.push_back(-3.577924);
v.push_back(0.185809);
v.push_back(0.153713);
v.push_back(-3.577924);
v.push_back(0.185809);
v.push_back(0.131700);
v.push_back(-3.577924);
v.push_back(0.223936);
v.push_back(0.139620);
v.push_back(-3.559187);
v.push_back(0.286472);
v.push_back(0.211831);
v.push_back(-3.559187);
v.push_back(0.286472);
v.push_back(0.247936);
v.push_back(-3.559187);
v.push_back(0.223936);
v.push_back(0.211831);
v.push_back(-3.559187);
v.push_back(0.161400);
v.push_back(0.139620);
v.push_back(-3.559187);
v.push_back(0.161400);
v.push_back(0.103515);
v.push_back(-3.559187);
v.push_back(0.223936);
v.push_back(1.122424);
v.push_back(-2.500039);
v.push_back(0.354093);
v.push_back(1.165400);
v.push_back(-2.504526);
v.push_back(0.281053);
v.push_back(1.143932);
v.push_back(-2.497735);
v.push_back(0.366607);
v.push_back(1.186907);
v.push_back(-2.502222);
v.push_back(0.293566);
v.push_back(1.138441);
v.push_back(-2.394479);
v.push_back(0.357032);
v.push_back(1.181417);
v.push_back(-2.398967);
v.push_back(0.283992);
v.push_back(1.116933);
v.push_back(-2.396783);
v.push_back(0.344519);
v.push_back(1.159909);
v.push_back(-2.401271);
v.push_back(0.271479);
v.push_back(1.160856);
v.push_back(-2.463450);
v.push_back(0.340668);
v.push_back(1.174376);
v.push_back(-2.464862);
v.push_back(0.317689);
v.push_back(1.172649);
v.push_back(-2.432377);
v.push_back(0.314677);
v.push_back(1.159555);
v.push_back(-2.431142);
v.push_back(0.337087);
v.push_back(1.287533);
v.push_back(-2.448942);
v.push_back(0.417889);
v.push_back(1.299819);
v.push_back(-2.438572);
v.push_back(0.376480);
v.push_back(1.288599);
v.push_back(-2.408185);
v.push_back(0.380735);
v.push_back(1.276314);
v.push_back(-2.418555);
v.push_back(0.422143);
v.push_back(1.331363);
v.push_back(-2.435871);
v.push_back(0.437190);
v.push_back(1.322284);
v.push_back(-2.421200);
v.push_back(0.355145);
v.push_back(1.310290);
v.push_back(-2.391250);
v.push_back(0.360288);
v.push_back(1.319369);
v.push_back(-2.405921);
v.push_back(0.442334);
v.push_back(1.413941);
v.push_back(-2.403933);
v.push_back(0.443780);
v.push_back(1.404861);
v.push_back(-2.389262);
v.push_back(0.361735);
v.push_back(1.392867);
v.push_back(-2.359312);
v.push_back(0.366879);
v.push_back(1.401947);
v.push_back(-2.373983);
v.push_back(0.448924);
v.push_back(1.292778);
v.push_back(-2.418975);
v.push_back(0.417225);
v.push_back(1.302937);
v.push_back(-2.410149);
v.push_back(0.381236);
v.push_back(1.326046);
v.push_back(-2.409209);
v.push_back(0.432800);
v.push_back(1.319014);
v.push_back(-2.397848);
v.push_back(0.369263);
v.push_back(1.389920);
v.push_back(-2.384495);
v.push_back(0.437843);
v.push_back(1.382889);
v.push_back(-2.373134);
v.push_back(0.374306);
v.push_back(1.137904);
v.push_back(-2.398973);
v.push_back(0.308878);
v.push_back(1.159411);
v.push_back(-2.396669);
v.push_back(0.321391);
v.push_back(1.166153);
v.push_back(-2.431831);
v.push_back(0.325874);
v.push_back(1.279482);
v.push_back(-2.414840);
v.push_back(0.407686);
v.push_back(1.294909);
v.push_back(-2.416013);
v.push_back(0.405413);
v.push_back(1.322615);
v.push_back(-2.403666);
v.push_back(0.401796);
v.push_back(1.420184);
v.push_back(-2.377920);
v.push_back(0.403147);
v.push_back(1.431211);
v.push_back(-2.365792);
v.push_back(0.405197);
v.push_back(1.432553);
v.push_back(-2.413212);
v.push_back(0.393210);
v.push_back(1.333966);
v.push_back(-2.452221);
v.push_back(0.391740);
v.push_back(1.292135);
v.push_back(-2.445602);
v.push_back(0.406167);
v.push_back(1.167453);
v.push_back(-2.464139);
v.push_back(0.329455);
v.push_back(1.164902);
v.push_back(-2.499924);
v.push_back(0.330966);
v.push_back(1.143395);
v.push_back(-2.502228);
v.push_back(0.318452);
v.push_back(1.286958);
v.push_back(-2.437293);
v.push_back(0.406516);
v.push_back(1.289345);
v.push_back(-2.435561);
v.push_back(0.400436);
v.push_back(1.283799);
v.push_back(-2.420541);
v.push_back(0.402539);
v.push_back(1.281430);
v.push_back(-2.422270);
v.push_back(0.408588);
v.push_back(1.279156);
v.push_back(-2.423921);
v.push_back(0.414382);
v.push_back(1.284684);
v.push_back(-2.438944);
v.push_back(0.412310);
v.push_back(1.225793);
v.push_back(-2.451412);
v.push_back(0.364448);
v.push_back(1.222254);
v.push_back(-2.427213);
v.push_back(0.363862);
v.push_back(1.217716);
v.push_back(-2.427746);
v.push_back(0.372617);
v.push_back(1.213169);
v.push_back(-2.428167);
v.push_back(0.381259);
v.push_back(1.216475);
v.push_back(-2.452275);
v.push_back(0.382158);
v.push_back(1.221022);
v.push_back(-2.451854);
v.push_back(0.373516);
v.push_back(0.963033);
v.push_back(-2.104731);
v.push_back(0.377963);
v.push_back(0.963739);
v.push_back(-2.058923);
v.push_back(0.292394);
v.push_back(1.021246);
v.push_back(-2.360477);
v.push_back(0.175202);
v.push_back(1.025303);
v.push_back(-2.322452);
v.push_back(0.064735);
v.push_back(1.031443);
v.push_back(-1.295589);
v.push_back(0.026867);
v.push_back(1.031443);
v.push_back(-1.276245);
v.push_back(0.203775);
v.push_back(1.031443);
v.push_back(-1.248391);
v.push_back(0.237200);
v.push_back(1.031443);
v.push_back(-1.120839);
v.push_back(0.240589);
v.push_back(1.031443);
v.push_back(-1.095371);
v.push_back(0.229539);
v.push_back(1.031443);
v.push_back(-1.108588);
v.push_back(0.249133);
v.push_back(1.031443);
v.push_back(-1.050900);
v.push_back(0.428777);
v.push_back(1.031443);
v.push_back(-1.021838);
v.push_back(0.442046);
v.push_back(1.031443);
v.push_back(-1.032221);
v.push_back(0.476223);
v.push_back(1.031443);
v.push_back(-0.988531);
v.push_back(0.545291);
v.push_back(1.031443);
v.push_back(-0.924229);
v.push_back(0.533246);
v.push_back(1.031443);
v.push_back(-0.925243);
v.push_back(0.412112);
v.push_back(0.997955);
v.push_back(-0.784032);
v.push_back(0.482265);
v.push_back(0.961015);
v.push_back(-0.798366);
v.push_back(0.538827);
v.push_back(0.916035);
v.push_back(-1.131551);
v.push_back(0.629017);
v.push_back(0.935458);
v.push_back(-1.516591);
v.push_back(0.667885);
v.push_back(1.034995);
v.push_back(-2.371559);
v.push_back(0.363557);
v.push_back(1.140934);
v.push_back(-2.394618);
v.push_back(-0.015811);
v.push_back(1.042154);
v.push_back(-2.645944);
v.push_back(0.195204);
v.push_back(1.032480);
v.push_back(-3.029875);
v.push_back(0.281485);
v.push_back(1.031686);
v.push_back(-3.268776);
v.push_back(0.210659);
v.push_back(1.100894);
v.push_back(-3.419832);
v.push_back(-0.007156);
v.push_back(1.058142);
v.push_back(-3.611537);
v.push_back(-0.007156);
v.push_back(1.058142);
v.push_back(-3.614132);
v.push_back(0.105561);
v.push_back(1.031443);
v.push_back(-3.478498);
v.push_back(0.233931);
v.push_back(1.034608);
v.push_back(-3.164988);
v.push_back(0.367427);
v.push_back(1.044935);
v.push_back(-2.909405);
v.push_back(0.445384);
v.push_back(1.043751);
v.push_back(-2.370386);
v.push_back(0.393172);
v.push_back(0.935458);
v.push_back(-1.525950);
v.push_back(0.703123);
v.push_back(0.916035);
v.push_back(-1.114701);
v.push_back(0.659034);
v.push_back(1.073166);
v.push_back(-0.704083);
v.push_back(0.549493);
v.push_back(1.146791);
v.push_back(-0.282661);
v.push_back(0.511429);
v.push_back(1.118565);
v.push_back(-0.090609);
v.push_back(0.378523);
v.push_back(1.028127);
v.push_back(-0.017588);
v.push_back(0.256014);
v.push_back(1.302450);
v.push_back(-0.272725);
v.push_back(0.212854);
v.push_back(1.364471);
v.push_back(-0.742655);
v.push_back(0.372432);
v.push_back(1.315942);
v.push_back(-0.935045);
v.push_back(0.128320);
v.push_back(1.146610);
v.push_back(-1.023064);
v.push_back(-0.046048);
v.push_back(1.371560);
v.push_back(-2.612969);
v.push_back(0.253162);
v.push_back(1.342827);
v.push_back(-2.568455);
v.push_back(-0.003469);
v.push_back(1.205524);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(1.195293);
v.push_back(-2.325799);
v.push_back(0.210149);
v.push_back(1.317160);
v.push_back(-2.987251);
v.push_back(0.490857);
v.push_back(1.346432);
v.push_back(-3.140682);
v.push_back(0.432154);
v.push_back(1.349881);
v.push_back(-3.290463);
v.push_back(0.254919);
v.push_back(1.271632);
v.push_back(-3.418578);
v.push_back(0.104449);
v.push_back(1.262858);
v.push_back(-3.393188);
v.push_back(-0.007156);
v.push_back(1.333511);
v.push_back(-3.185471);
v.push_back(-0.007156);
v.push_back(1.357104);
v.push_back(-3.091060);
v.push_back(0.337217);
v.push_back(1.370491);
v.push_back(-2.965753);
v.push_back(0.411659);
v.push_back(1.293362);
v.push_back(-1.079122);
v.push_back(-0.030699);
v.push_back(1.258798);
v.push_back(-0.308943);
v.push_back(0.491000);
v.push_back(1.262133);
v.push_back(-0.133172);
v.push_back(0.341560);
v.push_back(1.259901);
v.push_back(-0.778115);
v.push_back(0.492824);
v.push_back(1.126634);
v.push_back(-0.240992);
v.push_back(0.089374);
v.push_back(1.129097);
v.push_back(-0.073357);
v.push_back(0.117219);
v.push_back(1.218418);
v.push_back(-0.074698);
v.push_back(0.207510);
v.push_back(1.031443);
v.push_back(-1.037141);
v.push_back(0.195446);
v.push_back(1.247591);
v.push_back(-1.063455);
v.push_back(0.204596);
v.push_back(1.277952);
v.push_back(-0.982025);
v.push_back(0.386366);
v.push_back(1.031443);
v.push_back(-0.960873);
v.push_back(0.349901);
v.push_back(1.370767);
v.push_back(-0.969725);
v.push_back(0.173725);
v.push_back(1.377946);
v.push_back(-0.749190);
v.push_back(0.386768);
v.push_back(1.367555);
v.push_back(-0.333755);
v.push_back(0.375961);
v.push_back(1.324738);
v.push_back(-0.244227);
v.push_back(0.255415);
v.push_back(1.217016);
v.push_back(-0.218379);
v.push_back(0.152738);
v.push_back(1.064517);
v.push_back(-0.529311);
v.push_back(0.529713);
v.push_back(1.239475);
v.push_back(-0.549862);
v.push_back(0.529236);
v.push_back(1.383611);
v.push_back(-0.532760);
v.push_back(0.426001);
v.push_back(1.361538);
v.push_back(-0.534232);
v.push_back(0.415145);
v.push_back(1.231309);
v.push_back(-2.021386);
v.push_back(-0.002551);
v.push_back(1.210548);
v.push_back(-2.036724);
v.push_back(0.071738);
v.push_back(1.024064);
v.push_back(-2.040543);
v.push_back(0.018778);
v.push_back(1.026567);
v.push_back(-2.026747);
v.push_back(-0.004033);
v.push_back(0.986679);
v.push_back(-1.391378);
v.push_back(0.651580);
v.push_back(0.996454);
v.push_back(-1.387985);
v.push_back(0.683317);
v.push_back(1.031443);
v.push_back(-1.518217);
v.push_back(-0.002046);
v.push_back(1.196378);
v.push_back(-1.625082);
v.push_back(-0.003536);
v.push_back(1.196219);
v.push_back(-1.626360);
v.push_back(0.076337);
v.push_back(1.031443);
v.push_back(-1.498765);
v.push_back(0.026867);
v.push_back(1.151905);
v.push_back(-1.037141);
v.push_back(0.195446);
v.push_back(1.136101);
v.push_back(-1.423867);
v.push_back(0.077824);
v.push_back(1.122194);
v.push_back(-1.621897);
v.push_back(0.074964);
v.push_back(1.150016);
v.push_back(-2.042560);
v.push_back(0.069819);
v.push_back(1.156655);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(1.158202);
v.push_back(-2.323577);
v.push_back(0.209123);
v.push_back(1.156077);
v.push_back(-1.390671);
v.push_back(0.503985);
v.push_back(1.148873);
v.push_back(-1.276115);
v.push_back(0.555584);
v.push_back(1.192127);
v.push_back(-1.274665);
v.push_back(0.565373);
v.push_back(1.204057);
v.push_back(-1.396198);
v.push_back(0.514830);
v.push_back(1.135554);
v.push_back(-1.043489);
v.push_back(0.556955);
v.push_back(1.107913);
v.push_back(-2.351243);
v.push_back(0.332705);
v.push_back(1.130354);
v.push_back(-2.361173);
v.push_back(0.350132);
v.push_back(1.154221);
v.push_back(-2.939534);
v.push_back(0.498342);
v.push_back(1.138156);
v.push_back(-3.152961);
v.push_back(0.435687);
v.push_back(1.221931);
v.push_back(-3.427473);
v.push_back(0.266761);
v.push_back(1.195830);
v.push_back(-3.546065);
v.push_back(0.104449);
v.push_back(1.168302);
v.push_back(-3.537933);
v.push_back(-0.007156);
v.push_back(0.984979);
v.push_back(-1.491513);
v.push_back(0.650230);
v.push_back(1.073688);
v.push_back(-1.435365);
v.push_back(0.594151);
v.push_back(1.077716);
v.push_back(-1.331443);
v.push_back(0.594945);
v.push_back(1.096952);
v.push_back(-1.327967);
v.push_back(0.629858);
v.push_back(1.096210);
v.push_back(-1.441454);
v.push_back(0.631052);
v.push_back(0.995723);
v.push_back(-1.499806);
v.push_back(0.684696);
v.push_back(0.963255);
v.push_back(-1.710163);
v.push_back(0.645088);
v.push_back(1.015078);
v.push_back(-1.706746);
v.push_back(0.646460);
v.push_back(1.022975);
v.push_back(-1.721245);
v.push_back(0.674570);
v.push_back(0.962786);
v.push_back(-1.718123);
v.push_back(0.690895);
v.push_back(1.012670);
v.push_back(-2.375983);
v.push_back(0.342015);
v.push_back(0.873205);
v.push_back(-2.475131);
v.push_back(0.411308);
v.push_back(0.872449);
v.push_back(-2.479384);
v.push_back(0.435649);
v.push_back(0.871496);
v.push_back(-2.903164);
v.push_back(0.493586);
v.push_back(0.945410);
v.push_back(-3.173718);
v.push_back(0.431019);
v.push_back(0.988671);
v.push_back(-3.493051);
v.push_back(0.248852);
v.push_back(0.998091);
v.push_back(-3.609424);
v.push_back(0.131368);
v.push_back(0.994393);
v.push_back(-3.624381);
v.push_back(-0.007156);
v.push_back(0.957149);
v.push_back(-0.770148);
v.push_back(0.561802);
v.push_back(0.872436);
v.push_back(-1.073111);
v.push_back(0.648620);
v.push_back(0.870070);
v.push_back(-1.071552);
v.push_back(0.620301);
v.push_back(1.026807);
v.push_back(-0.813962);
v.push_back(0.552109);
v.push_back(1.025958);
v.push_back(-0.838472);
v.push_back(0.538828);
v.push_back(1.165789);
v.push_back(-1.043899);
v.push_back(0.574793);
v.push_back(0.828514);
v.push_back(-1.390224);
v.push_back(0.657614);
v.push_back(0.903717);
v.push_back(-1.389088);
v.push_back(0.710988);
v.push_back(0.833956);
v.push_back(-1.178303);
v.push_back(0.701253);
v.push_back(0.769338);
v.push_back(-1.103696);
v.push_back(0.648043);
v.push_back(0.768113);
v.push_back(-1.108858);
v.push_back(0.619892);
v.push_back(0.791922);
v.push_back(-1.175929);
v.push_back(0.629017);
v.push_back(0.995383);
v.push_back(-1.840608);
v.push_back(0.635255);
v.push_back(1.028976);
v.push_back(-1.834074);
v.push_back(0.618363);
v.push_back(0.995059);
v.push_back(-1.855102);
v.push_back(0.661676);
v.push_back(1.034055);
v.push_back(-1.844445);
v.push_back(0.646170);
v.push_back(0.811883);
v.push_back(-1.865862);
v.push_back(0.686269);
v.push_back(0.807377);
v.push_back(-1.728753);
v.push_back(0.711316);
v.push_back(0.807591);
v.push_back(-1.720793);
v.push_back(0.680101);
v.push_back(0.812031);
v.push_back(-1.851370);
v.push_back(0.659843);
v.push_back(1.205791);
v.push_back(-0.986319);
v.push_back(0.383949);
v.push_back(1.208496);
v.push_back(-1.068791);
v.push_back(0.270624);
v.push_back(1.123946);
v.push_back(-0.786802);
v.push_back(0.480514);
v.push_back(1.222811);
v.push_back(-1.144979);
v.push_back(0.440152);
v.push_back(1.234215);
v.push_back(-1.456803);
v.push_back(0.257471);
v.push_back(1.175430);
v.push_back(-1.453670);
v.push_back(0.254944);
v.push_back(1.053030);
v.push_back(-1.270085);
v.push_back(-0.046434);
v.push_back(1.232095);
v.push_back(-1.236264);
v.push_back(-0.026937);
v.push_back(1.186837);
v.push_back(-1.139792);
v.push_back(0.077133);
v.push_back(1.195604);
v.push_back(-1.563865);
v.push_back(0.123649);
v.push_back(1.131566);
v.push_back(-1.562341);
v.push_back(0.123414);
v.push_back(1.157693);
v.push_back(-1.231317);
v.push_back(0.115898);
v.push_back(1.031443);
v.push_back(-1.243803);
v.push_back(0.114517);
v.push_back(1.101161);
v.push_back(-1.032894);
v.push_back(0.471973);
v.push_back(0.885741);
v.push_back(-2.776860);
v.push_back(0.498546);
v.push_back(1.049238);
v.push_back(-2.654077);
v.push_back(0.437150);
v.push_back(1.123528);
v.push_back(-2.695718);
v.push_back(0.409832);
v.push_back(1.310172);
v.push_back(-2.721582);
v.push_back(0.448029);
v.push_back(1.387140);
v.push_back(-2.751805);
v.push_back(0.377337);
v.push_back(1.047892);
v.push_back(-2.808951);
v.push_back(0.258063);
v.push_back(1.353158);
v.push_back(-0.365493);
v.push_back(0.355766);
v.push_back(1.237342);
v.push_back(-2.354544);
v.push_back(-0.009461);
v.push_back(1.229225);
v.push_back(-2.492751);
v.push_back(0.075198);
v.push_back(1.212316);
v.push_back(-2.499173);
v.push_back(0.232726);
v.push_back(1.130986);
v.push_back(-0.163595);
v.push_back(0.095729);
v.push_back(1.218565);
v.push_back(-0.172423);
v.push_back(0.172763);
v.push_back(1.323950);
v.push_back(-0.205694);
v.push_back(0.310199);
v.push_back(1.363094);
v.push_back(-0.317187);
v.push_back(0.447141);
v.push_back(1.370833);
v.push_back(-0.532404);
v.push_back(0.494807);
v.push_back(1.356906);
v.push_back(-0.753248);
v.push_back(0.455066);
v.push_back(1.376886);
v.push_back(-0.999171);
v.push_back(0.292138);
v.push_back(1.326865);
v.push_back(-1.109970);
v.push_back(0.114258);
v.push_back(1.238951);
v.push_back(-1.202736);
v.push_back(0.065268);
v.push_back(1.205483);
v.push_back(-1.625497);
v.push_back(0.056741);
v.push_back(1.230850);
v.push_back(-2.026692);
v.push_back(0.057845);
v.push_back(1.229195);
v.push_back(-2.313357);
v.push_back(0.049964);
v.push_back(1.307585);
v.push_back(-2.501458);
v.push_back(0.078989);
v.push_back(1.321888);
v.push_back(-2.527011);
v.push_back(0.253414);
v.push_back(0.909405);
v.push_back(-0.014614);
v.push_back(0.131710);
v.push_back(0.911329);
v.push_back(-0.136548);
v.push_back(0.009332);
v.push_back(0.909564);
v.push_back(-0.209913);
v.push_back(-0.006744);
v.push_back(0.972759);
v.push_back(-0.257127);
v.push_back(0.192156);
v.push_back(0.981777);
v.push_back(-0.367784);
v.push_back(0.222208);
v.push_back(0.973333);
v.push_back(-0.495640);
v.push_back(0.240044);
v.push_back(0.980046);
v.push_back(-0.670942);
v.push_back(0.251646);
v.push_back(0.996475);
v.push_back(-0.802965);
v.push_back(0.215419);
v.push_back(0.917949);
v.push_back(-1.106456);
v.push_back(0.155693);
v.push_back(0.856823);
v.push_back(-1.374654);
v.push_back(-0.002046);
v.push_back(0.856823);
v.push_back(-1.517290);
v.push_back(-0.002046);
v.push_back(0.856823);
v.push_back(-2.017052);
v.push_back(-0.002046);
v.push_back(0.906114);
v.push_back(-2.359802);
v.push_back(-0.002507);
v.push_back(0.862925);
v.push_back(-2.442568);
v.push_back(0.187990);
v.push_back(0.856823);
v.push_back(-2.841912);
v.push_back(0.258188);
v.push_back(0.856823);
v.push_back(-3.117023);
v.push_back(0.281493);
v.push_back(0.856823);
v.push_back(-3.367006);
v.push_back(0.210661);
v.push_back(0.988185);
v.push_back(-3.485275);
v.push_back(-0.007156);
v.push_back(1.247004);
v.push_back(-3.194801);
v.push_back(-0.007156);
v.push_back(1.208012);
v.push_back(-3.131553);
v.push_back(0.279635);
v.push_back(1.246142);
v.push_back(-2.965584);
v.push_back(0.360348);
v.push_back(1.252648);
v.push_back(-2.776562);
v.push_back(0.327614);
v.push_back(1.242845);
v.push_back(-2.627140);
v.push_back(0.229537);
v.push_back(1.225170);
v.push_back(-2.568015);
v.push_back(-0.003445);
v.push_back(0.952875);
v.push_back(-0.027482);
v.push_back(0.252290);
v.push_back(0.862076);
v.push_back(-0.013018);
v.push_back(0.184080);
v.push_back(1.036095);
v.push_back(-0.112550);
v.push_back(0.362862);
v.push_back(1.052097);
v.push_back(-0.290281);
v.push_back(0.481142);
v.push_back(0.955543);
v.push_back(-0.513419);
v.push_back(0.508797);
v.push_back(0.859368);
v.push_back(-0.615700);
v.push_back(0.512753);
v.push_back(1.358493);
v.push_back(-2.521896);
v.push_back(0.064375);
v.push_back(1.377363);
v.push_back(-2.553540);
v.push_back(0.262590);
v.push_back(1.377254);
v.push_back(-2.726707);
v.push_back(0.432466);
v.push_back(1.376716);
v.push_back(-2.975591);
v.push_back(0.466490);
v.push_back(1.386859);
v.push_back(-3.115956);
v.push_back(0.389399);
v.push_back(1.376664);
v.push_back(-3.242014);
v.push_back(0.114670);
v.push_back(1.279693);
v.push_back(-3.410922);
v.push_back(0.045148);
v.push_back(1.190897);
v.push_back(-3.551581);
v.push_back(0.044256);
v.push_back(1.061813);
v.push_back(-3.626053);
v.push_back(0.047722);
v.push_back(0.998218);
v.push_back(-3.630784);
v.push_back(0.059451);
v.push_back(1.093668);
v.push_back(-3.441600);
v.push_back(0.281635);
v.push_back(1.031703);
v.push_back(-3.441600);
v.push_back(0.281635);
v.push_back(1.006859);
v.push_back(-3.428584);
v.push_back(0.223936);
v.push_back(1.031703);
v.push_back(-3.441600);
v.push_back(0.166237);
v.push_back(1.093668);
v.push_back(-3.441600);
v.push_back(0.166237);
v.push_back(1.118513);
v.push_back(-3.428584);
v.push_back(0.223936);
v.push_back(1.089444);
v.push_back(-3.559946);
v.push_back(0.270282);
v.push_back(1.035928);
v.push_back(-3.559946);
v.push_back(0.270282);
v.push_back(1.009170);
v.push_back(-3.559946);
v.push_back(0.223936);
v.push_back(1.035928);
v.push_back(-3.559946);
v.push_back(0.177590);
v.push_back(1.089444);
v.push_back(-3.559946);
v.push_back(0.177590);
v.push_back(1.116201);
v.push_back(-3.559946);
v.push_back(0.223936);
v.push_back(1.078755);
v.push_back(-3.585260);
v.push_back(0.253582);
v.push_back(1.046617);
v.push_back(-3.585260);
v.push_back(0.253582);
v.push_back(1.035180);
v.push_back(-3.587827);
v.push_back(0.223936);
v.push_back(1.046617);
v.push_back(-3.585260);
v.push_back(0.194289);
v.push_back(1.078755);
v.push_back(-3.585260);
v.push_back(0.194289);
v.push_back(1.090191);
v.push_back(-3.587827);
v.push_back(0.223936);
v.push_back(1.095408);
v.push_back(-3.568291);
v.push_back(0.280612);
v.push_back(1.029964);
v.push_back(-3.568291);
v.push_back(0.280612);
v.push_back(0.997242);
v.push_back(-3.568291);
v.push_back(0.223936);
v.push_back(1.029964);
v.push_back(-3.568291);
v.push_back(0.167260);
v.push_back(1.095408);
v.push_back(-3.568291);
v.push_back(0.167260);
v.push_back(1.128129);
v.push_back(-3.568291);
v.push_back(0.223936);
v.push_back(1.148228);
v.push_back(-0.351157);
v.push_back(0.518271);
v.push_back(1.194124);
v.push_back(-0.351157);
v.push_back(0.518271);
v.push_back(1.212526);
v.push_back(-0.433569);
v.push_back(0.475535);
v.push_back(1.194124);
v.push_back(-0.413691);
v.push_back(0.432798);
v.push_back(1.148228);
v.push_back(-0.413691);
v.push_back(0.432798);
v.push_back(1.129825);
v.push_back(-0.433569);
v.push_back(0.475535);
v.push_back(1.151357);
v.push_back(-0.155932);
v.push_back(0.509862);
v.push_back(1.190995);
v.push_back(-0.155932);
v.push_back(0.509862);
v.push_back(1.210814);
v.push_back(-0.155932);
v.push_back(0.475535);
v.push_back(1.190995);
v.push_back(-0.155932);
v.push_back(0.441207);
v.push_back(1.151357);
v.push_back(-0.155932);
v.push_back(0.441207);
v.push_back(1.131537);
v.push_back(-0.155932);
v.push_back(0.475535);
v.push_back(1.159274);
v.push_back(-0.131588);
v.push_back(0.497493);
v.push_back(1.183078);
v.push_back(-0.131588);
v.push_back(0.497493);
v.push_back(1.191549);
v.push_back(-0.129686);
v.push_back(0.475535);
v.push_back(1.183078);
v.push_back(-0.131588);
v.push_back(0.453576);
v.push_back(1.159274);
v.push_back(-0.131588);
v.push_back(0.453576);
v.push_back(1.150803);
v.push_back(-0.129686);
v.push_back(0.475535);
v.push_back(1.146939);
v.push_back(-0.144156);
v.push_back(0.517514);
v.push_back(1.141159);
v.push_back(-0.146807);
v.push_back(0.527433);
v.push_back(1.195412);
v.push_back(-0.144156);
v.push_back(0.517514);
v.push_back(1.201192);
v.push_back(-0.146807);
v.push_back(0.527433);
v.push_back(1.219649);
v.push_back(-0.144156);
v.push_back(0.475535);
v.push_back(1.231144);
v.push_back(-0.149775);
v.push_back(0.475482);
v.push_back(1.195412);
v.push_back(-0.144156);
v.push_back(0.433556);
v.push_back(1.201157);
v.push_back(-0.150299);
v.push_back(0.423593);
v.push_back(1.146939);
v.push_back(-0.144156);
v.push_back(0.433556);
v.push_back(1.141195);
v.push_back(-0.150299);
v.push_back(0.423593);
v.push_back(1.122703);
v.push_back(-0.144156);
v.push_back(0.475535);
v.push_back(1.111208);
v.push_back(-0.149775);
v.push_back(0.475482);
v.push_back(1.034720);
v.push_back(-0.823492);
v.push_back(0.478551);
v.push_back(1.098989);
v.push_back(-0.851288);
v.push_back(0.541428);
v.push_back(1.083775);
v.push_back(-0.863615);
v.push_back(0.525919);
v.push_back(1.055019);
v.push_back(-0.930858);
v.push_back(0.472690);
v.push_back(1.136402);
v.push_back(-0.807001);
v.push_back(0.524088);
v.push_back(1.208256);
v.push_back(-0.996463);
v.push_back(0.489120);
v.push_back(1.162034);
v.push_back(-1.569560);
v.push_back(0.127032);
v.push_back(1.203135);
v.push_back(-1.461312);
v.push_back(0.260222);
v.push_back(1.179034);
v.push_back(-1.400697);
v.push_back(0.508775);
v.push_back(1.086061);
v.push_back(-1.445064);
v.push_back(0.609400);
v.push_back(0.996129);
v.push_back(-1.498932);
v.push_back(0.664180);
v.push_back(1.026036);
v.push_back(-1.712729);
v.push_back(0.658493);
v.push_back(1.038661);
v.push_back(-1.838051);
v.push_back(0.630740);
v.push_back(1.122763);
v.push_back(-2.349854);
v.push_back(0.340978);
v.push_back(1.176528);
v.push_back(-2.317602);
v.push_back(0.211766);
v.push_back(1.180187);
v.push_back(-2.275416);
v.push_back(0.074770);
v.push_back(1.178989);
v.push_back(-2.039831);
v.push_back(0.078120);
v.push_back(1.157467);
v.push_back(-1.629162);
v.push_back(0.080931);
v.push_back(0.811050);
v.push_back(-1.805232);
v.push_back(0.711006);
v.push_back(0.964606);
v.push_back(-1.793895);
v.push_back(0.686101);
v.push_back(1.014030);
v.push_back(-1.851597);
v.push_back(0.659349);
v.push_back(1.000918);
v.push_back(-1.789770);
v.push_back(0.675553);
v.push_back(1.085797);
v.push_back(-2.367141);
v.push_back(0.377764);
v.push_back(1.129383);
v.push_back(-2.408738);
v.push_back(0.358620);
v.push_back(1.095015);
v.push_back(-2.408587);
v.push_back(0.374867);
v.push_back(1.202320);
v.push_back(-2.397367);
v.push_back(0.219469);
v.push_back(1.218518);
v.push_back(-2.396524);
v.push_back(0.072304);
v.push_back(1.284427);
v.push_back(-2.416159);
v.push_back(0.070414);
v.push_back(0.914429);
v.push_back(-3.475739);
v.push_back(0.107366);
v.push_back(1.061900);
v.push_back(-3.389982);
v.push_back(0.107366);
v.push_back(1.218552);
v.push_back(-3.201705);
v.push_back(0.173858);
v.push_back(1.344455);
v.push_back(-3.167883);
v.push_back(0.207267);
v.push_back(1.384133);
v.push_back(-3.209247);
v.push_back(0.260070);
v.push_back(1.352980);
v.push_back(-3.246625);
v.push_back(0.348742);
v.push_back(1.172130);
v.push_back(-3.323923);
v.push_back(0.355239);
v.push_back(1.031443);
v.push_back(-3.360304);
v.push_back(0.299884);
v.push_back(0.977097);
v.push_back(-3.374163);
v.push_back(0.344779);
v.push_back(1.014793);
v.push_back(-2.046117);
v.push_back(0.551629);
v.push_back(1.026045);
v.push_back(-2.198211);
v.push_back(0.473295);
v.push_back(1.052069);
v.push_back(-2.048837);
v.push_back(0.544868);
v.push_back(1.075659);
v.push_back(-2.200930);
v.push_back(0.459107);
v.push_back(1.085611);
v.push_back(-2.201298);
v.push_back(0.468036);
v.push_back(1.060918);
v.push_back(-2.054041);
v.push_back(0.553074);
v.push_back(1.013240);
v.push_back(-2.060602);
v.push_back(0.572843);
v.push_back(1.028373);
v.push_back(-2.220006);
v.push_back(0.488746);
v.push_back(1.056074);
v.push_back(-2.064425);
v.push_back(0.564485);
v.push_back(1.084309);
v.push_back(-2.210785);
v.push_back(0.478392);
v.push_back(1.057120);
v.push_back(-2.216674);
v.push_back(0.488598);
v.push_back(1.035832);
v.push_back(-2.067473);
v.push_back(0.572011);
v.push_back(0.794500);
v.push_back(-2.082741);
v.push_back(0.386180);
v.push_back(0.730052);
v.push_back(-2.033090);
v.push_back(0.301180);
v.push_back(0.708475);
v.push_back(-2.323279);
v.push_back(0.196732);
v.push_back(0.676534);
v.push_back(-2.348285);
v.push_back(0.064735);
v.push_back(0.719331);
v.push_back(-1.779973);
v.push_back(0.026196);
v.push_back(0.697941);
v.push_back(-1.495218);
v.push_back(0.026867);
v.push_back(0.692619);
v.push_back(-2.069375);
v.push_back(0.283606);
v.push_back(0.677836);
v.push_back(-1.963798);
v.push_back(0.151089);
v.push_back(0.683828);
v.push_back(-1.718724);
v.push_back(0.076091);
v.push_back(0.774562);
v.push_back(-1.625749);
v.push_back(0.026609);
v.push_back(0.990656);
v.push_back(-2.190158);
v.push_back(0.258782);
v.push_back(0.988855);
v.push_back(-2.255238);
v.push_back(0.356799);
v.push_back(0.805176);
v.push_back(-2.319337);
v.push_back(0.399678);
v.push_back(0.859430);
v.push_back(-2.443584);
v.push_back(0.408953);
v.push_back(1.043209);
v.push_back(-2.374978);
v.push_back(0.303964);
v.push_back(1.008306);
v.push_back(-2.222697);
v.push_back(0.324140);
v.push_back(0.994488);
v.push_back(-2.080982);
v.push_back(0.348864);
v.push_back(0.755612);
v.push_back(-2.043308);
v.push_back(0.356922);
v.push_back(0.954522);
v.push_back(-0.708194);
v.push_back(0.544073);
v.push_back(0.984714);
v.push_back(-0.516008);
v.push_back(0.525980);
v.push_back(1.076029);
v.push_back(-0.284011);
v.push_back(0.500126);
v.push_back(1.056603);
v.push_back(-0.099126);
v.push_back(0.375862);
v.push_back(0.972840);
v.push_back(-0.014266);
v.push_back(0.258144);
v.push_back(0.875418);
v.push_back(-0.001950);
v.push_back(0.168466);
v.push_back(1.206267);
v.push_back(-1.280079);
v.push_back(0.150962);
v.push_back(1.223729);
v.push_back(-1.340069);
v.push_back(0.265086);
v.push_back(1.203973);
v.push_back(-1.339459);
v.push_back(0.541293);
v.push_back(1.100196);
v.push_back(-1.388427);
v.push_back(0.635454);
v.push_back(0.998401);
v.push_back(-1.447172);
v.push_back(0.689750);
v.push_back(0.921534);
v.push_back(-1.461649);
v.push_back(0.712901);
v.push_back(1.142843);
v.push_back(-0.276497);
v.push_back(0.524608);
v.push_back(1.199508);
v.push_back(-0.276497);
v.push_back(0.524608);
v.push_back(1.227841);
v.push_back(-0.316698);
v.push_back(0.475535);
v.push_back(1.199508);
v.push_back(-0.323398);
v.push_back(0.426461);
v.push_back(1.142843);
v.push_back(-0.323398);
v.push_back(0.426461);
v.push_back(1.114511);
v.push_back(-0.316698);
v.push_back(0.475535);
v.push_back(1.154871);
v.push_back(-0.137021);
v.push_back(0.503775);
v.push_back(1.187480);
v.push_back(-0.137021);
v.push_back(0.503775);
v.push_back(1.203784);
v.push_back(-0.137021);
v.push_back(0.475535);
v.push_back(1.187480);
v.push_back(-0.137021);
v.push_back(0.447294);
v.push_back(1.154871);
v.push_back(-0.137021);
v.push_back(0.447294);
v.push_back(1.138567);
v.push_back(-0.137021);
v.push_back(0.475535);
v.push_back(1.100938);
v.push_back(-3.478788);
v.push_back(0.290190);
v.push_back(1.024434);
v.push_back(-3.478788);
v.push_back(0.290190);
v.push_back(0.986182);
v.push_back(-3.478788);
v.push_back(0.223936);
v.push_back(1.024434);
v.push_back(-3.478788);
v.push_back(0.157681);
v.push_back(1.100938);
v.push_back(-3.478788);
v.push_back(0.157681);
v.push_back(1.139190);
v.push_back(-3.478788);
v.push_back(0.223936);
v.push_back(1.084698);
v.push_back(-3.577924);
v.push_back(0.262063);
v.push_back(1.040673);
v.push_back(-3.577924);
v.push_back(0.262063);
v.push_back(1.018661);
v.push_back(-3.577924);
v.push_back(0.223936);
v.push_back(1.040673);
v.push_back(-3.577924);
v.push_back(0.185809);
v.push_back(1.084698);
v.push_back(-3.577924);
v.push_back(0.185809);
v.push_back(1.106711);
v.push_back(-3.577924);
v.push_back(0.223936);
v.push_back(1.098791);
v.push_back(-3.559187);
v.push_back(0.286472);
v.push_back(1.026581);
v.push_back(-3.559187);
v.push_back(0.286472);
v.push_back(0.990476);
v.push_back(-3.559187);
v.push_back(0.223936);
v.push_back(1.026581);
v.push_back(-3.559187);
v.push_back(0.161400);
v.push_back(1.098791);
v.push_back(-3.559187);
v.push_back(0.161400);
v.push_back(1.134896);
v.push_back(-3.559187);
v.push_back(0.223936);
v.push_back(0.578969);
v.push_back(-1.966663);
v.push_back(0.130456);
v.push_back(0.659397);
v.push_back(-1.966665);
v.push_back(0.130444);
v.push_back(0.578976);
v.push_back(-1.954869);
v.push_back(0.169668);
v.push_back(0.659403);
v.push_back(-1.954871);
v.push_back(0.169656);
v.push_back(0.578974);
v.push_back(-1.871358);
v.push_back(0.144551);
v.push_back(0.659401);
v.push_back(-1.871360);
v.push_back(0.144539);
v.push_back(0.578968);
v.push_back(-1.883152);
v.push_back(0.105339);
v.push_back(0.659395);
v.push_back(-1.883154);
v.push_back(0.105327);
v.push_back(0.595670);
v.push_back(-1.943940);
v.push_back(0.163469);
v.push_back(0.642708);
v.push_back(-1.943941);
v.push_back(0.163460);
v.push_back(0.642707);
v.push_back(-1.883425);
v.push_back(0.145259);
v.push_back(0.595669);
v.push_back(-1.883424);
v.push_back(0.145268);
v.push_back(0.597327);
v.push_back(-1.935378);
v.push_back(0.184224);
v.push_back(0.641058);
v.push_back(-1.935379);
v.push_back(0.184216);
v.push_back(0.641057);
v.push_back(-1.879117);
v.push_back(0.167294);
v.push_back(0.597326);
v.push_back(-1.879116);
v.push_back(0.167302);
v.push_back(0.606291);
v.push_back(-1.924898);
v.push_back(0.177253);
v.push_back(0.632093);
v.push_back(-1.924899);
v.push_back(0.177248);
v.push_back(0.632092);
v.push_back(-1.891703);
v.push_back(0.167264);
v.push_back(0.606290);
v.push_back(-1.891703);
v.push_back(0.167269);
v.push_back(0.606293);
v.push_back(-1.920336);
v.push_back(0.192421);
v.push_back(0.632095);
v.push_back(-1.920336);
v.push_back(0.192416);
v.push_back(0.632095);
v.push_back(-1.887141);
v.push_back(0.182432);
v.push_back(0.606293);
v.push_back(-1.887140);
v.push_back(0.182437);
v.push_back(0.616967);
v.push_back(-1.908529);
v.push_back(0.187790);
v.push_back(0.621422);
v.push_back(-1.908529);
v.push_back(0.187789);
v.push_back(0.621422);
v.push_back(-1.899542);
v.push_back(0.185087);
v.push_back(0.616967);
v.push_back(-1.899542);
v.push_back(0.185087);
v.push_back(0.616073);
v.push_back(-1.887080);
v.push_back(0.262257);
v.push_back(0.622344);
v.push_back(-1.887080);
v.push_back(0.262256);
v.push_back(0.622344);
v.push_back(-1.876355);
v.push_back(0.259030);
v.push_back(0.616073);
v.push_back(-1.876355);
v.push_back(0.259031);
v.push_back(0.602159);
v.push_back(-1.890260);
v.push_back(0.288871);
v.push_back(0.636266);
v.push_back(-1.890261);
v.push_back(0.288865);
v.push_back(0.636265);
v.push_back(-1.859178);
v.push_back(0.279516);
v.push_back(0.602158);
v.push_back(-1.859177);
v.push_back(0.279522);
v.push_back(0.602651);
v.push_back(-1.896631);
v.push_back(0.265963);
v.push_back(0.635765);
v.push_back(-1.896633);
v.push_back(0.265957);
v.push_back(0.635764);
v.push_back(-1.866500);
v.push_back(0.256894);
v.push_back(0.602650);
v.push_back(-1.866499);
v.push_back(0.256900);
v.push_back(0.983158);
v.push_back(-2.153801);
v.push_back(0.120589);
v.push_back(0.983158);
v.push_back(-2.159048);
v.push_back(0.112194);
v.push_back(0.983158);
v.push_back(-2.170592);
v.push_back(0.092255);
v.push_back(0.983158);
v.push_back(-2.176888);
v.push_back(0.067070);
v.push_back(0.983158);
v.push_back(-2.178462);
v.push_back(0.027165);
v.push_back(0.983158);
v.push_back(-2.162721);
v.push_back(0.030313);
v.push_back(0.983158);
v.push_back(-2.163770);
v.push_back(0.066021);
v.push_back(0.983158);
v.push_back(-2.159573);
v.push_back(0.088583);
v.push_back(0.983158);
v.push_back(-2.151178);
v.push_back(0.104848);
v.push_back(0.983158);
v.push_back(-2.144357);
v.push_back(0.116391);
v.push_back(0.997977);
v.push_back(-2.153801);
v.push_back(0.120589);
v.push_back(0.997977);
v.push_back(-2.159048);
v.push_back(0.112194);
v.push_back(0.997977);
v.push_back(-2.170592);
v.push_back(0.092255);
v.push_back(0.997977);
v.push_back(-2.176888);
v.push_back(0.067070);
v.push_back(0.997977);
v.push_back(-2.178462);
v.push_back(0.027165);
v.push_back(0.997977);
v.push_back(-2.162721);
v.push_back(0.030313);
v.push_back(0.997977);
v.push_back(-2.163770);
v.push_back(0.066021);
v.push_back(0.997977);
v.push_back(-2.159573);
v.push_back(0.088583);
v.push_back(0.997977);
v.push_back(-2.151178);
v.push_back(0.104848);
v.push_back(0.997977);
v.push_back(-2.144357);
v.push_back(0.116391);
v.push_back(0.958153);
v.push_back(-2.192351);
v.push_back(0.139012);
v.push_back(0.963431);
v.push_back(-2.123419);
v.push_back(0.099208);
v.push_back(0.958131);
v.push_back(-2.180056);
v.push_back(0.150351);
v.push_back(0.963413);
v.push_back(-2.116896);
v.push_back(0.108973);
v.push_back(1.017722);
v.push_back(-2.116814);
v.push_back(0.109055);
v.push_back(1.023002);
v.push_back(-2.179975);
v.push_back(0.150433);
v.push_back(1.022980);
v.push_back(-2.192270);
v.push_back(0.139093);
v.push_back(1.017704);
v.push_back(-2.123338);
v.push_back(0.099289);
v.push_back(0.863482);
v.push_back(-2.155812);
v.push_back(0.123773);
v.push_back(0.863482);
v.push_back(-2.161058);
v.push_back(0.115377);
v.push_back(0.863482);
v.push_back(-2.172602);
v.push_back(0.095439);
v.push_back(0.863482);
v.push_back(-2.178898);
v.push_back(0.070254);
v.push_back(0.863598);
v.push_back(-2.180386);
v.push_back(0.030700);
v.push_back(0.863598);
v.push_back(-2.164645);
v.push_back(0.033848);
v.push_back(0.863482);
v.push_back(-2.165781);
v.push_back(0.069204);
v.push_back(0.863482);
v.push_back(-2.161583);
v.push_back(0.091766);
v.push_back(0.863482);
v.push_back(-2.153188);
v.push_back(0.108032);
v.push_back(0.863482);
v.push_back(-2.146367);
v.push_back(0.119575);
v.push_back(0.878301);
v.push_back(-2.155812);
v.push_back(0.123773);
v.push_back(0.878301);
v.push_back(-2.161058);
v.push_back(0.115377);
v.push_back(0.878301);
v.push_back(-2.172602);
v.push_back(0.095439);
v.push_back(0.878301);
v.push_back(-2.178898);
v.push_back(0.070254);
v.push_back(0.878417);
v.push_back(-2.180386);
v.push_back(0.030700);
v.push_back(0.878417);
v.push_back(-2.164645);
v.push_back(0.033848);
v.push_back(0.878301);
v.push_back(-2.165781);
v.push_back(0.069204);
v.push_back(0.878301);
v.push_back(-2.161583);
v.push_back(0.091766);
v.push_back(0.878301);
v.push_back(-2.153188);
v.push_back(0.108032);
v.push_back(0.878301);
v.push_back(-2.146367);
v.push_back(0.119575);
v.push_back(0.820287);
v.push_back(-2.194361);
v.push_back(0.142195);
v.push_back(0.825565);
v.push_back(-2.136395);
v.push_back(0.108487);
v.push_back(0.820266);
v.push_back(-2.182067);
v.push_back(0.153535);
v.push_back(0.825547);
v.push_back(-2.129871);
v.push_back(0.118253);
v.push_back(0.898046);
v.push_back(-2.129790);
v.push_back(0.118334);
v.push_back(0.903326);
v.push_back(-2.181985);
v.push_back(0.153616);
v.push_back(0.903304);
v.push_back(-2.194280);
v.push_back(0.142277);
v.push_back(0.898028);
v.push_back(-2.136313);
v.push_back(0.108569);
v.push_back(0.743166);
v.push_back(-2.163250);
v.push_back(0.129385);
v.push_back(0.743166);
v.push_back(-2.167106);
v.push_back(0.123215);
v.push_back(0.743166);
v.push_back(-2.175590);
v.push_back(0.108560);
v.push_back(0.743166);
v.push_back(-2.180218);
v.push_back(0.076732);
v.push_back(0.743166);
v.push_back(-2.170030);
v.push_back(0.031640);
v.push_back(0.743166);
v.push_back(-2.158461);
v.push_back(0.033954);
v.push_back(0.743166);
v.push_back(-2.170577);
v.push_back(0.075961);
v.push_back(0.743166);
v.push_back(-2.167492);
v.push_back(0.105861);
v.push_back(0.743166);
v.push_back(-2.161322);
v.push_back(0.117816);
v.push_back(0.743166);
v.push_back(-2.156308);
v.push_back(0.126300);
v.push_back(0.754058);
v.push_back(-2.163250);
v.push_back(0.129385);
v.push_back(0.754058);
v.push_back(-2.167106);
v.push_back(0.123215);
v.push_back(0.754058);
v.push_back(-2.175590);
v.push_back(0.108560);
v.push_back(0.754058);
v.push_back(-2.180218);
v.push_back(0.076732);
v.push_back(0.754058);
v.push_back(-2.170030);
v.push_back(0.031640);
v.push_back(0.754058);
v.push_back(-2.158461);
v.push_back(0.033954);
v.push_back(0.754058);
v.push_back(-2.170577);
v.push_back(0.075961);
v.push_back(0.754058);
v.push_back(-2.167492);
v.push_back(0.105861);
v.push_back(0.754058);
v.push_back(-2.161322);
v.push_back(0.117816);
v.push_back(0.754058);
v.push_back(-2.156308);
v.push_back(0.126300);
v.push_back(0.711419);
v.push_back(-2.191583);
v.push_back(0.142925);
v.push_back(0.715298);
v.push_back(-2.148979);
v.push_back(0.118150);
v.push_back(0.711403);
v.push_back(-2.182546);
v.push_back(0.151259);
v.push_back(0.715285);
v.push_back(-2.144184);
v.push_back(0.125328);
v.push_back(0.768569);
v.push_back(-2.144125);
v.push_back(0.125388);
v.push_back(0.772450);
v.push_back(-2.182487);
v.push_back(0.151319);
v.push_back(0.772434);
v.push_back(-2.191523);
v.push_back(0.142985);
v.push_back(0.768556);
v.push_back(-2.148919);
v.push_back(0.118210);
v.push_back(0.594660);
v.push_back(-1.806652);
v.push_back(0.688146);
v.push_back(0.594660);
v.push_back(-1.914714);
v.push_back(0.612447);
v.push_back(0.504212);
v.push_back(-1.914830);
v.push_back(0.572631);
v.push_back(0.594660);
v.push_back(-1.914830);
v.push_back(0.567224);
v.push_back(0.639884);
v.push_back(-1.914830);
v.push_back(0.567224);
v.push_back(0.730332);
v.push_back(-1.914830);
v.push_back(0.572631);
v.push_back(0.639884);
v.push_back(-1.914714);
v.push_back(0.612447);
v.push_back(0.639884);
v.push_back(-1.806652);
v.push_back(0.688146);
v.push_back(0.594660);
v.push_back(-1.832947);
v.push_back(0.683667);
v.push_back(0.594660);
v.push_back(-1.926442);
v.push_back(0.612603);
v.push_back(0.504212);
v.push_back(-1.922892);
v.push_back(0.572786);
v.push_back(0.594660);
v.push_back(-1.934212);
v.push_back(0.567379);
v.push_back(0.639884);
v.push_back(-1.934212);
v.push_back(0.567379);
v.push_back(0.730332);
v.push_back(-1.922892);
v.push_back(0.572786);
v.push_back(0.639884);
v.push_back(-1.926442);
v.push_back(0.612602);
v.push_back(0.639884);
v.push_back(-1.832947);
v.push_back(0.683667);
v.push_back(0.639884);
v.push_back(-1.803674);
v.push_back(0.670666);
v.push_back(0.594660);
v.push_back(-1.803674);
v.push_back(0.670666);
v.push_back(0.623941);
v.push_back(-1.809319);
v.push_back(0.666790);
v.push_back(0.610602);
v.push_back(-1.809319);
v.push_back(0.666790);
v.push_back(0.594660);
v.push_back(-1.829969);
v.push_back(0.666187);
v.push_back(0.610602);
v.push_back(-1.823360);
v.push_back(0.664398);
v.push_back(0.639884);
v.push_back(-1.829969);
v.push_back(0.666187);
v.push_back(0.623941);
v.push_back(-1.823360);
v.push_back(0.664398);
v.push_back(0.522661);
v.push_back(-1.914714);
v.push_back(0.612447);
v.push_back(0.504212);
v.push_back(-1.914761);
v.push_back(0.593998);
v.push_back(0.504212);
v.push_back(-1.922983);
v.push_back(0.594153);
v.push_back(0.522661);
v.push_back(-1.926442);
v.push_back(0.612603);
v.push_back(0.730332);
v.push_back(-1.914761);
v.push_back(0.593998);
v.push_back(0.711882);
v.push_back(-1.914714);
v.push_back(0.612447);
v.push_back(0.711882);
v.push_back(-1.926442);
v.push_back(0.612602);
v.push_back(0.730332);
v.push_back(-1.922983);
v.push_back(0.594153);
v.push_back(0.638630);
v.push_back(-1.917804);
v.push_back(0.608654);
v.push_back(0.638630);
v.push_back(-1.917900);
v.push_back(0.571099);
v.push_back(0.706638);
v.push_back(-1.917804);
v.push_back(0.608654);
v.push_back(0.724065);
v.push_back(-1.917843);
v.push_back(0.593333);
v.push_back(0.724065);
v.push_back(-1.917900);
v.push_back(0.576506);
v.push_back(0.595913);
v.push_back(-1.917900);
v.push_back(0.571099);
v.push_back(0.595913);
v.push_back(-1.917804);
v.push_back(0.608654);
v.push_back(0.510478);
v.push_back(-1.917900);
v.push_back(0.576506);
v.push_back(0.510478);
v.push_back(-1.917843);
v.push_back(0.593333);
v.push_back(0.527905);
v.push_back(-1.917804);
v.push_back(0.608654);
v.push_back(0.508915);
v.push_back(-1.914821);
v.push_back(0.576627);
v.push_back(0.509840);
v.push_back(-1.914820);
v.push_back(0.592414);
v.push_back(0.526160);
v.push_back(-1.914962);
v.push_back(0.607495);
v.push_back(0.595205);
v.push_back(-1.915157);
v.push_back(0.607003);
v.push_back(0.639339);
v.push_back(-1.915157);
v.push_back(0.607003);
v.push_back(0.708384);
v.push_back(-1.914962);
v.push_back(0.607495);
v.push_back(0.724703);
v.push_back(-1.914820);
v.push_back(0.592414);
v.push_back(0.725628);
v.push_back(-1.914821);
v.push_back(0.576627);
v.push_back(0.639339);
v.push_back(-1.915245);
v.push_back(0.572680);
v.push_back(0.595205);
v.push_back(-1.915245);
v.push_back(0.572680);
v.push_back(0.638630);
v.push_back(-1.917847);
v.push_back(0.591968);
v.push_back(0.595913);
v.push_back(-1.917847);
v.push_back(0.591968);
v.push_back(0.594660);
v.push_back(-1.934148);
v.push_back(0.592422);
v.push_back(0.639884);
v.push_back(-1.934148);
v.push_back(0.592422);
v.push_back(0.623551);
v.push_back(-1.837278);
v.push_back(0.653214);
v.push_back(0.623183);
v.push_back(-1.871172);
v.push_back(0.631931);
v.push_back(0.610992);
v.push_back(-1.837278);
v.push_back(0.653214);
v.push_back(0.611360);
v.push_back(-1.871172);
v.push_back(0.631931);
v.push_back(0.623414);
v.push_back(-1.832480);
v.push_back(0.646784);
v.push_back(0.623184);
v.push_back(-1.866632);
v.push_back(0.625986);
v.push_back(0.611130);
v.push_back(-1.832480);
v.push_back(0.646784);
v.push_back(0.611359);
v.push_back(-1.866632);
v.push_back(0.625986);
v.push_back(0.610740);
v.push_back(-1.908439);
v.push_back(0.622000);
v.push_back(0.604907);
v.push_back(-1.918747);
v.push_back(0.618161);
v.push_back(0.623803);
v.push_back(-1.908439);
v.push_back(0.621999);
v.push_back(0.629637);
v.push_back(-1.918747);
v.push_back(0.618161);
v.push_back(0.610734);
v.push_back(-1.904382);
v.push_back(0.617300);
v.push_back(0.600273);
v.push_back(-1.913213);
v.push_back(0.613856);
v.push_back(0.623810);
v.push_back(-1.904382);
v.push_back(0.617300);
v.push_back(0.634271);
v.push_back(-1.913213);
v.push_back(0.613856);
v.push_back(0.880801);
v.push_back(-1.925396);
v.push_back(0.408995);
v.push_back(0.847592);
v.push_back(-1.925396);
v.push_back(0.408995);
v.push_back(0.830988);
v.push_back(-1.913147);
v.push_back(0.382975);
v.push_back(0.847592);
v.push_back(-1.900899);
v.push_back(0.356954);
v.push_back(0.880801);
v.push_back(-1.900899);
v.push_back(0.356954);
v.push_back(0.897405);
v.push_back(-1.913147);
v.push_back(0.382975);
v.push_back(0.880801);
v.push_back(-1.938634);
v.push_back(0.402764);
v.push_back(0.847592);
v.push_back(-1.938634);
v.push_back(0.402764);
v.push_back(0.830988);
v.push_back(-1.926385);
v.push_back(0.376743);
v.push_back(0.847592);
v.push_back(-1.914137);
v.push_back(0.350722);
v.push_back(0.880801);
v.push_back(-1.914137);
v.push_back(0.350722);
v.push_back(0.897405);
v.push_back(-1.926385);
v.push_back(0.376743);
v.push_back(0.870059);
v.push_back(-1.922557);
v.push_back(0.395520);
v.push_back(0.858334);
v.push_back(-1.922557);
v.push_back(0.395520);
v.push_back(0.858334);
v.push_back(-1.930058);
v.push_back(0.391989);
v.push_back(0.870059);
v.push_back(-1.930058);
v.push_back(0.391989);
v.push_back(0.847964);
v.push_back(-1.914907);
v.push_back(0.379270);
v.push_back(0.853827);
v.push_back(-1.910583);
v.push_back(0.370083);
v.push_back(0.853827);
v.push_back(-1.918084);
v.push_back(0.366552);
v.push_back(0.847964);
v.push_back(-1.922409);
v.push_back(0.375739);
v.push_back(0.874566);
v.push_back(-1.910583);
v.push_back(0.370083);
v.push_back(0.880429);
v.push_back(-1.914907);
v.push_back(0.379270);
v.push_back(0.880429);
v.push_back(-1.922409);
v.push_back(0.375739);
v.push_back(0.874566);
v.push_back(-1.918084);
v.push_back(0.366551);
v.push_back(0.880659);
v.push_back(-1.956176);
v.push_back(0.466941);
v.push_back(0.847734);
v.push_back(-1.956176);
v.push_back(0.466941);
v.push_back(0.847734);
v.push_back(-1.963678);
v.push_back(0.463410);
v.push_back(0.880659);
v.push_back(-1.963678);
v.push_back(0.463410);
v.push_back(0.774302);
v.push_back(-1.902007);
v.push_back(0.351865);
v.push_back(0.790764);
v.push_back(-1.889863);
v.push_back(0.326067);
v.push_back(0.790764);
v.push_back(-1.897365);
v.push_back(0.322535);
v.push_back(0.774302);
v.push_back(-1.909509);
v.push_back(0.348334);
v.push_back(0.937629);
v.push_back(-1.889863);
v.push_back(0.326067);
v.push_back(0.954091);
v.push_back(-1.902007);
v.push_back(0.351865);
v.push_back(0.954091);
v.push_back(-1.909509);
v.push_back(0.348334);
v.push_back(0.937629);
v.push_back(-1.897365);
v.push_back(0.322536);
v.push_back(0.871114);
v.push_back(-1.932781);
v.push_back(0.386975);
v.push_back(0.857279);
v.push_back(-1.932781);
v.push_back(0.386975);
v.push_back(0.850361);
v.push_back(-1.927678);
v.push_back(0.376135);
v.push_back(0.857279);
v.push_back(-1.922575);
v.push_back(0.365294);
v.push_back(0.871114);
v.push_back(-1.922575);
v.push_back(0.365294);
v.push_back(0.878032);
v.push_back(-1.927678);
v.push_back(0.376135);
v.push_back(0.871080);
v.push_back(-2.020888);
v.push_back(0.345436);
v.push_back(0.857335);
v.push_back(-2.020894);
v.push_back(0.345448);
v.push_back(0.850429);
v.push_back(-2.015799);
v.push_back(0.334625);
v.push_back(0.857313);
v.push_back(-2.010732);
v.push_back(0.323861);
v.push_back(0.871080);
v.push_back(-2.010732);
v.push_back(0.323861);
v.push_back(0.877964);
v.push_back(-2.015810);
v.push_back(0.334649);
v.push_back(0.882624);
v.push_back(-2.407495);
v.push_back(0.185551);
v.push_back(0.845768);
v.push_back(-2.407495);
v.push_back(0.185551);
v.push_back(0.827340);
v.push_back(-2.393901);
v.push_back(0.156673);
v.push_back(0.845768);
v.push_back(-2.380307);
v.push_back(0.127794);
v.push_back(0.882624);
v.push_back(-2.380307);
v.push_back(0.127794);
v.push_back(0.901052);
v.push_back(-2.393901);
v.push_back(0.156673);
v.push_back(0.884206);
v.push_back(-2.030605);
v.push_back(0.365990);
v.push_back(0.844209);
v.push_back(-2.030610);
v.push_back(0.366002);
v.push_back(0.824177);
v.push_back(-2.015833);
v.push_back(0.334610);
v.push_back(0.844187);
v.push_back(-2.001083);
v.push_back(0.303275);
v.push_back(0.884206);
v.push_back(-2.001083);
v.push_back(0.303276);
v.push_back(0.904216);
v.push_back(-2.015844);
v.push_back(0.334633);
v.push_back(0.942699);
v.push_back(-1.939069);
v.push_back(0.420866);
v.push_back(0.909520);
v.push_back(-1.953200);
v.push_back(0.450886);
v.push_back(0.864196);
v.push_back(-1.958373);
v.push_back(0.461874);
v.push_back(0.818873);
v.push_back(-1.953200);
v.push_back(0.450886);
v.push_back(0.785694);
v.push_back(-1.939069);
v.push_back(0.420866);
v.push_back(0.773549);
v.push_back(-1.919766);
v.push_back(0.379859);
v.push_back(0.785694);
v.push_back(-1.900463);
v.push_back(0.338851);
v.push_back(0.818873);
v.push_back(-1.886332);
v.push_back(0.308832);
v.push_back(0.864196);
v.push_back(-1.881160);
v.push_back(0.297844);
v.push_back(0.909520);
v.push_back(-1.886332);
v.push_back(0.308832);
v.push_back(0.942699);
v.push_back(-1.900463);
v.push_back(0.338851);
v.push_back(0.954844);
v.push_back(-1.919766);
v.push_back(0.379859);
v.push_back(0.949210);
v.push_back(-1.950032);
v.push_back(0.419861);
v.push_back(0.913279);
v.push_back(-1.965334);
v.push_back(0.452370);
v.push_back(0.864196);
v.push_back(-1.970936);
v.push_back(0.464269);
v.push_back(0.815114);
v.push_back(-1.965334);
v.push_back(0.452370);
v.push_back(0.779183);
v.push_back(-1.950032);
v.push_back(0.419861);
v.push_back(0.766032);
v.push_back(-1.929127);
v.push_back(0.375452);
v.push_back(0.779183);
v.push_back(-1.908224);
v.push_back(0.331044);
v.push_back(0.815114);
v.push_back(-1.892921);
v.push_back(0.298535);
v.push_back(0.864196);
v.push_back(-1.887319);
v.push_back(0.286636);
v.push_back(0.913279);
v.push_back(-1.892921);
v.push_back(0.298535);
v.push_back(0.949210);
v.push_back(-1.908224);
v.push_back(0.331044);
v.push_back(0.962361);
v.push_back(-1.929127);
v.push_back(0.375452);
v.push_back(0.959743);
v.push_back(-1.949046);
v.push_back(0.427046);
v.push_back(0.919360);
v.push_back(-1.966245);
v.push_back(0.463583);
v.push_back(0.864196);
v.push_back(-1.972540);
v.push_back(0.476957);
v.push_back(0.809033);
v.push_back(-1.966245);
v.push_back(0.463583);
v.push_back(0.768650);
v.push_back(-1.949046);
v.push_back(0.427046);
v.push_back(0.753869);
v.push_back(-1.925552);
v.push_back(0.377135);
v.push_back(0.768650);
v.push_back(-1.902058);
v.push_back(0.327225);
v.push_back(0.809033);
v.push_back(-1.884859);
v.push_back(0.290688);
v.push_back(0.864196);
v.push_back(-1.878563);
v.push_back(0.277314);
v.push_back(0.919360);
v.push_back(-1.884859);
v.push_back(0.290688);
v.push_back(0.959743);
v.push_back(-1.902058);
v.push_back(0.327225);
v.push_back(0.974524);
v.push_back(-1.925552);
v.push_back(0.377135);
v.push_back(0.959743);
v.push_back(-1.937475);
v.push_back(0.432493);
v.push_back(0.919360);
v.push_back(-1.954674);
v.push_back(0.469030);
v.push_back(0.864196);
v.push_back(-1.960969);
v.push_back(0.482404);
v.push_back(0.809033);
v.push_back(-1.954674);
v.push_back(0.469030);
v.push_back(0.768650);
v.push_back(-1.937475);
v.push_back(0.432493);
v.push_back(0.753869);
v.push_back(-1.913981);
v.push_back(0.382582);
v.push_back(0.768650);
v.push_back(-1.890486);
v.push_back(0.332672);
v.push_back(0.809033);
v.push_back(-1.873287);
v.push_back(0.296134);
v.push_back(0.864196);
v.push_back(-1.866992);
v.push_back(0.282761);
v.push_back(0.919360);
v.push_back(-1.873287);
v.push_back(0.296134);
v.push_back(0.959743);
v.push_back(-1.890486);
v.push_back(0.332672);
v.push_back(0.974524);
v.push_back(-1.913981);
v.push_back(0.382582);
v.push_back(0.949210);
v.push_back(-1.931309);
v.push_back(0.428674);
v.push_back(0.913279);
v.push_back(-1.946612);
v.push_back(0.461183);
v.push_back(0.864196);
v.push_back(-1.952213);
v.push_back(0.473082);
v.push_back(0.815114);
v.push_back(-1.946612);
v.push_back(0.461183);
v.push_back(0.779183);
v.push_back(-1.931309);
v.push_back(0.428674);
v.push_back(0.766032);
v.push_back(-1.910405);
v.push_back(0.384265);
v.push_back(0.779183);
v.push_back(-1.889501);
v.push_back(0.339857);
v.push_back(0.815114);
v.push_back(-1.874198);
v.push_back(0.307348);
v.push_back(0.864196);
v.push_back(-1.868597);
v.push_back(0.295449);
v.push_back(0.913279);
v.push_back(-1.874198);
v.push_back(0.307348);
v.push_back(0.949210);
v.push_back(-1.889501);
v.push_back(0.339857);
v.push_back(0.962361);
v.push_back(-1.910405);
v.push_back(0.384265);
v.push_back(0.857305);
v.push_back(-1.976979);
v.push_back(0.339735);
v.push_back(0.871088);
v.push_back(-1.976979);
v.push_back(0.339735);
v.push_back(0.877979);
v.push_back(-1.982063);
v.push_back(0.350535);
v.push_back(0.871088);
v.push_back(-1.987146);
v.push_back(0.361334);
v.push_back(0.857305);
v.push_back(-1.987146);
v.push_back(0.361334);
v.push_back(0.850414);
v.push_back(-1.982063);
v.push_back(0.350535);
v.push_back(0.876546);
v.push_back(-1.991181);
v.push_back(0.369883);
v.push_back(0.851847);
v.push_back(-1.991181);
v.push_back(0.369883);
v.push_back(0.851877);
v.push_back(-2.053155);
v.push_back(0.340654);
v.push_back(0.876516);
v.push_back(-2.053155);
v.push_back(0.340654);
v.push_back(0.839498);
v.push_back(-1.982071);
v.push_back(0.350531);
v.push_back(0.839557);
v.push_back(-2.044067);
v.push_back(0.321347);
v.push_back(0.851847);
v.push_back(-1.972961);
v.push_back(0.331178);
v.push_back(0.851877);
v.push_back(-2.034979);
v.push_back(0.302041);
v.push_back(0.876546);
v.push_back(-1.972961);
v.push_back(0.331178);
v.push_back(0.876516);
v.push_back(-2.034979);
v.push_back(0.302041);
v.push_back(0.888896);
v.push_back(-1.982071);
v.push_back(0.350531);
v.push_back(0.888836);
v.push_back(-2.044067);
v.push_back(0.321348);
v.push_back(0.738506);
v.push_back(-1.684914);
v.push_back(0.018266);
v.push_back(0.956649);
v.push_back(-1.684914);
v.push_back(0.018266);
v.push_back(0.738506);
v.push_back(-1.677224);
v.push_back(0.105944);
v.push_back(0.956649);
v.push_back(-1.677224);
v.push_back(0.105944);
v.push_back(0.738506);
v.push_back(-1.521433);
v.push_back(0.079361);
v.push_back(0.956649);
v.push_back(-1.521433);
v.push_back(0.079361);
v.push_back(0.738990);
v.push_back(-1.527127);
v.push_back(0.021690);
v.push_back(0.956165);
v.push_back(-1.527127);
v.push_back(0.021690);
v.push_back(0.837443);
v.push_back(-1.371088);
v.push_back(0.434154);
v.push_back(0.857713);
v.push_back(-1.371088);
v.push_back(0.434154);
v.push_back(0.837443);
v.push_back(-1.371088);
v.push_back(0.473061);
v.push_back(0.857713);
v.push_back(-1.371088);
v.push_back(0.473061);
v.push_back(0.837443);
v.push_back(-1.350818);
v.push_back(0.473061);
v.push_back(0.857713);
v.push_back(-1.350818);
v.push_back(0.473061);
v.push_back(0.837443);
v.push_back(-1.350818);
v.push_back(0.434154);
v.push_back(0.857713);
v.push_back(-1.350818);
v.push_back(0.434154);
v.push_back(0.847578);
v.push_back(-1.295774);
v.push_back(0.607103);
v.push_back(0.847578);
v.push_back(-1.316762);
v.push_back(0.615815);
v.push_back(0.847578);
v.push_back(-1.352876);
v.push_back(0.587918);
v.push_back(0.847578);
v.push_back(-1.383501);
v.push_back(0.481404);
v.push_back(0.847578);
v.push_back(-1.388854);
v.push_back(0.438793);
v.push_back(0.847578);
v.push_back(-1.428250);
v.push_back(0.375971);
v.push_back(0.847578);
v.push_back(-1.467053);
v.push_back(0.241521);
v.push_back(0.847578);
v.push_back(-1.478094);
v.push_back(0.148098);
v.push_back(0.847578);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.847578);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.847578);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.847578);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.847578);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.847578);
v.push_back(-1.768569);
v.push_back(0.089704);
v.push_back(0.847578);
v.push_back(-1.694822);
v.push_back(0.100807);
v.push_back(0.847578);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.897077);
v.push_back(-1.296014);
v.push_back(0.601970);
v.push_back(0.892067);
v.push_back(-1.317034);
v.push_back(0.610018);
v.push_back(0.919583);
v.push_back(-1.353100);
v.push_back(0.583139);
v.push_back(0.946363);
v.push_back(-1.383559);
v.push_back(0.481404);
v.push_back(0.974666);
v.push_back(-1.380430);
v.push_back(0.438793);
v.push_back(0.998597);
v.push_back(-1.419826);
v.push_back(0.383970);
v.push_back(1.049925);
v.push_back(-1.467053);
v.push_back(0.256093);
v.push_back(1.008833);
v.push_back(-1.478094);
v.push_back(0.173450);
v.push_back(0.998597);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.998597);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.998597);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.998597);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.998597);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.997927);
v.push_back(-1.768569);
v.push_back(0.089703);
v.push_back(0.997927);
v.push_back(-1.694822);
v.push_back(0.100806);
v.push_back(0.997927);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.997927);
v.push_back(-1.498519);
v.push_back(0.072683);
v.push_back(0.847578);
v.push_back(-1.478463);
v.push_back(0.070148);
v.push_back(0.847578);
v.push_back(-1.414415);
v.push_back(0.246810);
v.push_back(0.847578);
v.push_back(-1.364889);
v.push_back(0.365134);
v.push_back(1.049028);
v.push_back(-1.414415);
v.push_back(0.261318);
v.push_back(0.997927);
v.push_back(-1.373562);
v.push_back(0.375390);
v.push_back(1.008117);
v.push_back(-1.414877);
v.push_back(0.168361);
v.push_back(0.847578);
v.push_back(-1.416977);
v.push_back(0.168361);
v.push_back(0.974102);
v.push_back(-1.342276);
v.push_back(0.437860);
v.push_back(0.847578);
v.push_back(-1.333308);
v.push_back(0.437860);
v.push_back(0.925395);
v.push_back(-1.308347);
v.push_back(0.571002);
v.push_back(0.847578);
v.push_back(-1.308497);
v.push_back(0.574136);
v.push_back(0.847578);
v.push_back(-1.338499);
v.push_back(0.481404);
v.push_back(0.947375);
v.push_back(-1.340262);
v.push_back(0.481404);
v.push_back(0.997927);
v.push_back(-1.728492);
v.push_back(0.095737);
v.push_back(0.847578);
v.push_back(-1.730697);
v.push_back(0.095405);
v.push_back(0.769761);
v.push_back(-1.308347);
v.push_back(0.571002);
v.push_back(0.798078);
v.push_back(-1.296014);
v.push_back(0.601970);
v.push_back(0.803089);
v.push_back(-1.317034);
v.push_back(0.610018);
v.push_back(0.775573);
v.push_back(-1.353100);
v.push_back(0.583139);
v.push_back(0.748792);
v.push_back(-1.383559);
v.push_back(0.481404);
v.push_back(0.720490);
v.push_back(-1.380430);
v.push_back(0.438793);
v.push_back(0.696558);
v.push_back(-1.419826);
v.push_back(0.383970);
v.push_back(0.645230);
v.push_back(-1.467053);
v.push_back(0.256093);
v.push_back(0.686323);
v.push_back(-1.478094);
v.push_back(0.173450);
v.push_back(0.696558);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.696558);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.696558);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.696558);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.696558);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.697228);
v.push_back(-1.768569);
v.push_back(0.089703);
v.push_back(0.697228);
v.push_back(-1.694822);
v.push_back(0.100806);
v.push_back(0.697228);
v.push_back(-1.728492);
v.push_back(0.095737);
v.push_back(0.697228);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.697228);
v.push_back(-1.498519);
v.push_back(0.072683);
v.push_back(0.646128);
v.push_back(-1.414415);
v.push_back(0.261318);
v.push_back(0.687038);
v.push_back(-1.414877);
v.push_back(0.168361);
v.push_back(0.721053);
v.push_back(-1.342275);
v.push_back(0.437860);
v.push_back(0.697228);
v.push_back(-1.373562);
v.push_back(0.375390);
v.push_back(0.747780);
v.push_back(-1.340262);
v.push_back(0.481404);
v.push_back(0.847578);
v.push_back(-1.378363);
v.push_back(0.443885);
v.push_back(0.847578);
v.push_back(-1.372886);
v.push_back(0.466048);
v.push_back(0.928611);
v.push_back(-1.372473);
v.push_back(0.466048);
v.push_back(0.928611);
v.push_back(-1.372471);
v.push_back(0.443885);
v.push_back(0.928611);
v.push_back(-1.349664);
v.push_back(0.443885);
v.push_back(0.928611);
v.push_back(-1.350250);
v.push_back(0.466048);
v.push_back(0.847578);
v.push_back(-1.347070);
v.push_back(0.466048);
v.push_back(0.847578);
v.push_back(-1.346336);
v.push_back(0.443885);
v.push_back(0.766545);
v.push_back(-1.372471);
v.push_back(0.443885);
v.push_back(0.766545);
v.push_back(-1.372473);
v.push_back(0.466048);
v.push_back(0.766545);
v.push_back(-1.350250);
v.push_back(0.466048);
v.push_back(0.766545);
v.push_back(-1.349664);
v.push_back(0.443885);
v.push_back(0.281762);
v.push_back(-1.684914);
v.push_back(0.018266);
v.push_back(0.499905);
v.push_back(-1.684914);
v.push_back(0.018266);
v.push_back(0.281762);
v.push_back(-1.677224);
v.push_back(0.105944);
v.push_back(0.499905);
v.push_back(-1.677224);
v.push_back(0.105944);
v.push_back(0.281762);
v.push_back(-1.521433);
v.push_back(0.079361);
v.push_back(0.499905);
v.push_back(-1.521433);
v.push_back(0.079361);
v.push_back(0.282246);
v.push_back(-1.527127);
v.push_back(0.021690);
v.push_back(0.499421);
v.push_back(-1.527127);
v.push_back(0.021690);
v.push_back(0.380698);
v.push_back(-1.371088);
v.push_back(0.434154);
v.push_back(0.400969);
v.push_back(-1.371088);
v.push_back(0.434154);
v.push_back(0.380698);
v.push_back(-1.371088);
v.push_back(0.473061);
v.push_back(0.400969);
v.push_back(-1.371088);
v.push_back(0.473061);
v.push_back(0.380698);
v.push_back(-1.350818);
v.push_back(0.473061);
v.push_back(0.400969);
v.push_back(-1.350818);
v.push_back(0.473061);
v.push_back(0.380698);
v.push_back(-1.350818);
v.push_back(0.434154);
v.push_back(0.400969);
v.push_back(-1.350818);
v.push_back(0.434154);
v.push_back(0.390834);
v.push_back(-1.295774);
v.push_back(0.607103);
v.push_back(0.390834);
v.push_back(-1.316762);
v.push_back(0.615815);
v.push_back(0.390834);
v.push_back(-1.352876);
v.push_back(0.587918);
v.push_back(0.390834);
v.push_back(-1.383501);
v.push_back(0.481404);
v.push_back(0.390834);
v.push_back(-1.388854);
v.push_back(0.438793);
v.push_back(0.390834);
v.push_back(-1.428250);
v.push_back(0.375971);
v.push_back(0.390834);
v.push_back(-1.467053);
v.push_back(0.241521);
v.push_back(0.390834);
v.push_back(-1.478094);
v.push_back(0.148098);
v.push_back(0.390834);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.390834);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.390834);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.390834);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.390834);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.390834);
v.push_back(-1.768569);
v.push_back(0.089704);
v.push_back(0.390834);
v.push_back(-1.694822);
v.push_back(0.100807);
v.push_back(0.390834);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.440333);
v.push_back(-1.296014);
v.push_back(0.601970);
v.push_back(0.435322);
v.push_back(-1.317034);
v.push_back(0.610018);
v.push_back(0.462839);
v.push_back(-1.353100);
v.push_back(0.583139);
v.push_back(0.489619);
v.push_back(-1.383559);
v.push_back(0.481404);
v.push_back(0.517922);
v.push_back(-1.380430);
v.push_back(0.438793);
v.push_back(0.541853);
v.push_back(-1.419826);
v.push_back(0.383970);
v.push_back(0.593181);
v.push_back(-1.467053);
v.push_back(0.256093);
v.push_back(0.552088);
v.push_back(-1.478094);
v.push_back(0.173450);
v.push_back(0.541853);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.541853);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.541853);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.541853);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.541853);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.541183);
v.push_back(-1.768569);
v.push_back(0.089703);
v.push_back(0.541183);
v.push_back(-1.694822);
v.push_back(0.100806);
v.push_back(0.541183);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.541183);
v.push_back(-1.498519);
v.push_back(0.072683);
v.push_back(0.390834);
v.push_back(-1.478463);
v.push_back(0.070148);
v.push_back(0.390834);
v.push_back(-1.414415);
v.push_back(0.246810);
v.push_back(0.390834);
v.push_back(-1.364889);
v.push_back(0.365134);
v.push_back(0.592284);
v.push_back(-1.414415);
v.push_back(0.261318);
v.push_back(0.541183);
v.push_back(-1.373562);
v.push_back(0.375390);
v.push_back(0.551373);
v.push_back(-1.414877);
v.push_back(0.168361);
v.push_back(0.390834);
v.push_back(-1.416977);
v.push_back(0.168361);
v.push_back(0.517358);
v.push_back(-1.342276);
v.push_back(0.437860);
v.push_back(0.390834);
v.push_back(-1.333308);
v.push_back(0.437860);
v.push_back(0.468651);
v.push_back(-1.308347);
v.push_back(0.571002);
v.push_back(0.390834);
v.push_back(-1.308497);
v.push_back(0.574136);
v.push_back(0.390834);
v.push_back(-1.338499);
v.push_back(0.481404);
v.push_back(0.490631);
v.push_back(-1.340262);
v.push_back(0.481404);
v.push_back(0.541183);
v.push_back(-1.728492);
v.push_back(0.095737);
v.push_back(0.390834);
v.push_back(-1.730697);
v.push_back(0.095405);
v.push_back(0.313017);
v.push_back(-1.308347);
v.push_back(0.571002);
v.push_back(0.341334);
v.push_back(-1.296014);
v.push_back(0.601970);
v.push_back(0.346345);
v.push_back(-1.317034);
v.push_back(0.610018);
v.push_back(0.318828);
v.push_back(-1.353100);
v.push_back(0.583139);
v.push_back(0.292048);
v.push_back(-1.383559);
v.push_back(0.481404);
v.push_back(0.263745);
v.push_back(-1.380430);
v.push_back(0.438793);
v.push_back(0.239814);
v.push_back(-1.419826);
v.push_back(0.383970);
v.push_back(0.188486);
v.push_back(-1.467053);
v.push_back(0.256093);
v.push_back(0.229579);
v.push_back(-1.478094);
v.push_back(0.173450);
v.push_back(0.239814);
v.push_back(-1.454831);
v.push_back(0.105980);
v.push_back(0.239814);
v.push_back(-1.501089);
v.push_back(0.127204);
v.push_back(0.239814);
v.push_back(-1.698349);
v.push_back(0.160145);
v.push_back(0.239814);
v.push_back(-1.754779);
v.push_back(0.151785);
v.push_back(0.239814);
v.push_back(-1.779859);
v.push_back(0.109985);
v.push_back(0.240484);
v.push_back(-1.768569);
v.push_back(0.089703);
v.push_back(0.240484);
v.push_back(-1.694822);
v.push_back(0.100806);
v.push_back(0.240484);
v.push_back(-1.728492);
v.push_back(0.095737);
v.push_back(0.240484);
v.push_back(-1.435653);
v.push_back(0.064735);
v.push_back(0.240484);
v.push_back(-1.498519);
v.push_back(0.072683);
v.push_back(0.189384);
v.push_back(-1.414415);
v.push_back(0.261318);
v.push_back(0.230294);
v.push_back(-1.414877);
v.push_back(0.168361);
v.push_back(0.264309);
v.push_back(-1.342275);
v.push_back(0.437860);
v.push_back(0.240484);
v.push_back(-1.373562);
v.push_back(0.375390);
v.push_back(0.291036);
v.push_back(-1.340262);
v.push_back(0.481404);
v.push_back(0.390834);
v.push_back(-1.378363);
v.push_back(0.443885);
v.push_back(0.390834);
v.push_back(-1.372886);
v.push_back(0.466048);
v.push_back(0.471866);
v.push_back(-1.372473);
v.push_back(0.466048);
v.push_back(0.471866);
v.push_back(-1.372471);
v.push_back(0.443885);
v.push_back(0.471866);
v.push_back(-1.349664);
v.push_back(0.443885);
v.push_back(0.471866);
v.push_back(-1.350250);
v.push_back(0.466048);
v.push_back(0.390834);
v.push_back(-1.347070);
v.push_back(0.466048);
v.push_back(0.390834);
v.push_back(-1.346336);
v.push_back(0.443885);
v.push_back(0.309801);
v.push_back(-1.372471);
v.push_back(0.443885);
v.push_back(0.309801);
v.push_back(-1.372473);
v.push_back(0.466048);
v.push_back(0.309801);
v.push_back(-1.350250);
v.push_back(0.466048);
v.push_back(0.309801);
v.push_back(-1.349664);
v.push_back(0.443885);
v.push_back(0.907253);
v.push_back(-2.181283);
v.push_back(0.429990);
v.push_back(0.838359);
v.push_back(-2.176738);
v.push_back(0.455746);
v.push_back(0.767548);
v.push_back(-2.172193);
v.push_back(0.435856);
v.push_back(0.736301);
v.push_back(-2.170310);
v.push_back(0.381972);
v.push_back(0.933873);
v.push_back(-2.183166);
v.push_back(0.373676);
v.push_back(0.886060);
v.push_back(-2.271687);
v.push_back(0.422769);
v.push_back(0.831879);
v.push_back(-2.268112);
v.push_back(0.443024);
v.push_back(0.776189);
v.push_back(-2.264538);
v.push_back(0.427382);
v.push_back(0.751615);
v.push_back(-2.263057);
v.push_back(0.385004);
v.push_back(0.906996);
v.push_back(-2.273168);
v.push_back(0.378480);
v.push_back(0.860016);
v.push_back(-2.341552);
v.push_back(0.399491);
v.push_back(0.821969);
v.push_back(-2.351309);
v.push_back(0.397144);
v.push_back(0.785148);
v.push_back(-2.338345);
v.push_back(0.400338);
v.push_back(0.769006);
v.push_back(-2.323832);
v.push_back(0.387950);
v.push_back(0.882196);
v.push_back(-2.331198);
v.push_back(0.383197);
v.push_back(0.842354);
v.push_back(-2.113899);
v.push_back(0.453517);
v.push_back(0.773719);
v.push_back(-2.109493);
v.push_back(0.434239);
v.push_back(0.743432);
v.push_back(-2.107668);
v.push_back(0.382010);
v.push_back(0.934934);
v.push_back(-2.120130);
v.push_back(0.373969);
v.push_back(0.909131);
v.push_back(-2.118305);
v.push_back(0.428553);
v.push_back(0.916738);
v.push_back(-2.113498);
v.push_back(0.432174);
v.push_back(0.923750);
v.push_back(-2.120298);
v.push_back(0.435935);
v.push_back(0.842913);
v.push_back(-2.109035);
v.push_back(0.459276);
v.push_back(0.842814);
v.push_back(-2.115011);
v.push_back(0.466182);
v.push_back(0.767128);
v.push_back(-2.103763);
v.push_back(0.438456);
v.push_back(0.759634);
v.push_back(-2.109618);
v.push_back(0.442826);
v.push_back(0.722890);
v.push_back(-2.107235);
v.push_back(0.379519);
v.push_back(0.731888);
v.push_back(-2.100669);
v.push_back(0.380521);
v.push_back(0.947122);
v.push_back(-2.114675);
v.push_back(0.371484);
v.push_back(0.955079);
v.push_back(-2.122345);
v.push_back(0.369770);
v.push_back(0.032910);
v.push_back(-2.232923);
v.push_back(0.107551);
v.push_back(0.038967);
v.push_back(-2.274622);
v.push_back(0.230427);
v.push_back(0.028928);
v.push_back(-2.014354);
v.push_back(0.109709);
v.push_back(0.043535);
v.push_back(-1.645203);
v.push_back(0.112483);
v.push_back(0.095955);
v.push_back(-1.621897);
v.push_back(0.074964);
v.push_back(0.068133);
v.push_back(-2.042560);
v.push_back(0.069819);
v.push_back(0.061494);
v.push_back(-2.279743);
v.push_back(0.068792);
v.push_back(0.059947);
v.push_back(-2.323577);
v.push_back(0.209123);
v.push_back(0.062072);
v.push_back(-1.390671);
v.push_back(0.503985);
v.push_back(0.026541);
v.push_back(-1.439150);
v.push_back(0.493781);
v.push_back(0.114838);
v.push_back(-2.352190);
v.push_back(0.341155);
v.push_back(0.079252);
v.push_back(-2.212864);
v.push_back(0.340564);
v.push_back(0.004270);
v.push_back(-1.493204);
v.push_back(0.270655);
v.push_back(0.042719);
v.push_back(-1.453670);
v.push_back(0.254945);
v.push_back(0.043243);
v.push_back(-1.583237);
v.push_back(0.152629);
v.push_back(0.086583);
v.push_back(-1.562341);
v.push_back(0.123414);
v.push_back(0.042160);
v.push_back(-1.569560);
v.push_back(0.127032);
v.push_back(0.001059);
v.push_back(-1.461312);
v.push_back(0.260222);
v.push_back(0.025161);
v.push_back(-1.400697);
v.push_back(0.508775);
v.push_back(0.102917);
v.push_back(-2.350801);
v.push_back(0.349428);
v.push_back(0.044549);
v.push_back(-2.317602);
v.push_back(0.211766);
v.push_back(0.040891);
v.push_back(-2.275416);
v.push_back(0.074770);
v.push_back(0.042089);
v.push_back(-2.039831);
v.push_back(0.078120);
v.push_back(0.046727);
v.push_back(-1.629162);
v.push_back(0.080931);
v.push_back(0.053963);
v.push_back(-1.936357);
v.push_back(0.361076);
v.push_back(0.005730);
v.push_back(-1.964357);
v.push_back(0.240676);
v.push_back(-0.008341);
v.push_back(-1.652524);
v.push_back(0.253759);
v.push_back(0.042312);
v.push_back(-1.661617);
v.push_back(0.406213);
v.push_back(0.074846);
v.push_back(-1.933941);
v.push_back(0.362016);
v.push_back(0.059995);
v.push_back(-1.655905);
v.push_back(0.403308);
v.push_back(0.103217);
v.push_back(-2.216333);
v.push_back(0.342988);
v.push_back(0.061577);
v.push_back(-1.435313);
v.push_back(0.487460);
v.push_back(0.089108);
v.push_back(-2.283690);
v.push_back(0.340778);
v.push_back(0.106800);
v.push_back(-2.285208);
v.push_back(0.337738);
v.push_back(0.126846);
v.push_back(-2.281246);
v.push_back(0.400629);
v.push_back(0.143907);
v.push_back(-2.282764);
v.push_back(0.395046);
v.push_back(0.098238);
v.push_back(-2.258845);
v.push_back(0.365851);
v.push_back(0.121734);
v.push_back(-2.260331);
v.push_back(0.359688);
v.push_back(0.070850);
v.push_back(-2.220509);
v.push_back(0.326930);
v.push_back(0.047235);
v.push_back(-1.939823);
v.push_back(0.346171);
v.push_back(0.035284);
v.push_back(-1.660491);
v.push_back(0.387340);
v.push_back(0.020368);
v.push_back(-1.445842);
v.push_back(0.466160);
v.push_back(0.022177);
v.push_back(-1.408201);
v.push_back(0.478006);
v.push_back(0.059676);
v.push_back(-1.398470);
v.push_back(0.473155);
v.push_back(0.074313);
v.push_back(-2.253986);
v.push_back(0.214456);
v.push_back(0.057279);
v.push_back(-1.677794);
v.push_back(0.244248);
v.push_back(0.076906);
v.push_back(-1.981175);
v.push_back(0.230509);
v.push_back(0.048309);
v.push_back(-1.539699);
v.push_back(0.250825);
v.push_back(0.125574);
v.push_back(-1.649877);
v.push_back(0.212224);
v.push_back(0.121477);
v.push_back(-2.012577);
v.push_back(0.203145);
v.push_back(0.113514);
v.push_back(-2.027807);
v.push_back(0.132088);
v.push_back(0.129473);
v.push_back(-1.636803);
v.push_back(0.139563);
v.push_back(0.092634);
v.push_back(-1.721164);
v.push_back(0.229428);
v.push_back(0.097984);
v.push_back(-1.950117);
v.push_back(0.221293);
v.push_back(0.100585);
v.push_back(-1.595819);
v.push_back(0.158371);
v.push_back(0.078870);
v.push_back(-2.207737);
v.push_back(0.137477);
v.push_back(0.059882);
v.push_back(-1.448650);
v.push_back(0.457225);
v.push_back(0.059648);
v.push_back(-1.658702);
v.push_back(0.382985);
v.push_back(0.075109);
v.push_back(-1.939977);
v.push_back(0.345214);
v.push_back(0.099524);
v.push_back(-2.221144);
v.push_back(0.326565);
v.push_back(0.089796);
v.push_back(-2.296562);
v.push_back(0.277177);
v.push_back(0.088023);
v.push_back(-2.338212);
v.push_back(0.276654);
v.push_back(0.074403);
v.push_back(-2.334583);
v.push_back(0.282177);
v.push_back(0.068429);
v.push_back(-2.300825);
v.push_back(0.285040);
SetVertices(&v[0], v.size());
    Subdivide();
    int* triangles = 0;
    int triangleCount = 0;
    GetSubdividedTriangles(&triangles, &triangleCount);
    float * vertices = 0;
    int vertexCount = 0;
    GetSubdividedVertices(&vertices, &vertexCount);

}