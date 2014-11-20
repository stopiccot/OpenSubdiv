#include <stdio.h>

#include <osd/error.h>
#include <osdutil/adaptiveEvaluator.h>
#include <osdutil/uniformEvaluator.h>
#include <osdutil/topology.h>


#if _MSC_VER // this is defined when compiling with Visual Studio
    #define STDCALL __stdcall
	#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
    #define STDCALL
	#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

typedef void (STDCALL *UnityDebugLog)(bool error, char* text);

extern "C" {
	void EXPORT_API Initialize(bool enableCodegen);
	void EXPORT_API SetUnityDebugLog(UnityDebugLog f);
	bool EXPORT_API SetVertices(float* vertices, int vertexCount);
	bool EXPORT_API SetFaces(int* indicies, int* faces, int faceCount);
    bool EXPORT_API SetSubdivisionLevel(int level);
	bool EXPORT_API Subdivide();
	void EXPORT_API GetSubdividedVertices(float** vertices, int* vertexCount);
	void EXPORT_API GetSubdividedTriangles(int** triangles, int* triangleCount);
}

static void callbackError(OpenSubdiv::OsdErrorType err, const char *message)
{
    printf("OsdError: %d\n", err);
    printf("%s", message);
}

OpenSubdiv::OsdUtilSubdivTopology topology;
OpenSubdiv::OsdUtilUniformEvaluator uniformEvaluator;
std::string errorMessage;
FILE* codegen = NULL;
UnityDebugLog unityDebugLog = NULL;
const float* refinedTopologyPositions = NULL;
OpenSubdiv::OsdUtilSubdivTopology refinedTopology;

void SetUnityDebugLog(UnityDebugLog f) {
	unityDebugLog = f;
}

void ReportError(const std::string& errorText) {
    //FILE* f = fopen("/Users/stopiccot/subdiv_error.txt", "w");
    //fprintf(f, "Refine failed with \"%s\"\n", errorText.c_str());
    //fclose(f);

	if (unityDebugLog) {
		char buf[1024];
		sprintf(buf, "ERROR: %s", errorText.c_str());
		unityDebugLog(true, buf);
	}
}

void Initialize(bool enableCodegen) {

    enableCodegen = false;
    
    if (enableCodegen) {
    	codegen = fopen("/Users/stopiccot/codegen.cpp", "w");
    }
}

bool SetFaces(int* indicies, int* faces, int faceCount) {
    if (unityDebugLog) {
        unityDebugLog(false, "SetFaces****");
    }
    
    for (int i = 0; i < faceCount / 2; i++) {
        topology.AddFace(faces[2 * i + 1], &indicies[faces[2 * i + 0]]);
    }

	return true;
}

bool SetSubdivisionLevel(int level) {
    char buf[1024];
    sprintf(buf, "SetSubdivisionLevel: %d", level);
    unityDebugLog(false, buf);
    
    topology = OpenSubdiv::OsdUtilSubdivTopology();
    uniformEvaluator = OpenSubdiv::OsdUtilUniformEvaluator();
    
    topology.refinementLevel = level;

	return true;
}

bool SetVertices(float* vertices, int vertexCount) {
	if (codegen) {
		fprintf(codegen, "std::vector<float> v;\n");
		for (int i = 0; i < vertexCount; i++) {
			fprintf(codegen, "v.push_back(%f);\n", vertices[i]);
		}
		fprintf(codegen, "SetVertices(&v[0], v.size());\n");
        fflush(codegen);
	}
    
    topology.numVertices = vertexCount / 3;

	// Create uniformEvaluator
    if (!uniformEvaluator.Initialize(topology, &errorMessage)) {
        ReportError(errorMessage);
        return false;
    }

	uniformEvaluator.SetCoarsePositions(vertices, vertexCount, &errorMessage);

	return true;
}

bool Subdivide() {
	if (unityDebugLog) {
		unityDebugLog(false, "SUBDIVIDING888");
	}

	if (!uniformEvaluator.Refine(1, &errorMessage)) {
		ReportError(errorMessage);
		return false;
	}

	if (!uniformEvaluator.GetRefinedTopology(&refinedTopology, &refinedTopologyPositions, &errorMessage)) {
		ReportError(errorMessage);
		return false;
	}

//	if (!refinedTopology.WriteObjFile("/Users/stopiccot/code/other/OpenSubdiv/examples/unity3d/UnityProject/Assets/subdiv_output.obj", refinedTopologyPositions, &errorMessage)) {
//		ReportError(errorMessage);
//		return;
//	}

	return true;
}

void GetSubdividedVertices(float** vertices, int* vertexCount) {
	*vertexCount = 3 * refinedTopology.numVertices;
	*vertices = new float[*vertexCount];
    
    for (int i = 0; i < *vertexCount; i++) {
        (*vertices)[i] = refinedTopologyPositions[i];
    }
}

void GetSubdividedTriangles(int** triangles, int* triangleCount) {
    for (size_t i = 0; i < refinedTopology.nverts.size(); i++) {
        *triangleCount = (*triangleCount) + (refinedTopology.nverts[i] - 2) * 3;
    }
    
    *triangles = new int[*triangleCount];
    
    int k = 0, idx = 0;
    for (size_t i = 0; i < refinedTopology.nverts.size(); i++) {
        for (int t = 0; t < refinedTopology.nverts[i] - 2; t++) {
            (*triangles)[3 * k + 0] = refinedTopology.indices[idx + 0];
            (*triangles)[3 * k + 1] = refinedTopology.indices[idx + 1 + t];
            (*triangles)[3 * k + 2] = refinedTopology.indices[idx + 2 + t];
            k++;
        }
        idx += refinedTopology.nverts[i];
    }
}