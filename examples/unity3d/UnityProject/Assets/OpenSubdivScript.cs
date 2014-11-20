using UnityEngine;
using System.Collections;
using System;
using System.Runtime.InteropServices;
using System.IO;
using UnityExtension;
using System.Collections.Generic;

public class OpenSubdivScript : MonoBehaviour {

	OBJData objData;

	GameObject go = null;

	MeshFilter GetMeshFilter() {
		if (go == null) {
			go = new GameObject("Mesh", typeof(MeshFilter), typeof(MeshRenderer), typeof(Material));
			go.GetComponent<MeshRenderer>().material = new Material(Shader.Find("Diffuse"));
		}

		return go.GetComponent<MeshFilter>();
	}
	
	void LoadOBJMesh() {
		var stream = File.OpenRead(Application.streamingAssetsPath + "/Car.obj");
		objData = OBJLoader.LoadOBJ(stream);
		GetMeshFilter().mesh.LoadOBJ(objData);
		stream.Close();
	}

	OpenSubdiv.Mesh osdMesh = new OpenSubdiv.Mesh();
	OpenSubdiv.SubdividedMesh osdSubdividedMesh;

	void TestOpenSubdiv() {
		OpenSubdiv.Mesh.Initialize(true);

		if (!true) {
			osdMesh.vertices = objData.m_Vertices;

			foreach (var group in objData.m_Groups) {
				foreach (var face in group.Faces) {
					osdMesh.faces.Add (new OpenSubdiv.Face () { startIndex = osdMesh.indices.Count, vertexCount = face.Count });
					for (int i = 0; i < face.Count; i++) {
						osdMesh.indices.Add (face [i].m_VertexIndex);
					}
				}
			}
		} else {
			osdMesh.vertices.Add(new Vector3(-1.0f,  1.0f, -1.0f));
			osdMesh.vertices.Add(new Vector3(-1.0f,  1.0f,  1.0f));
			osdMesh.vertices.Add(new Vector3( 1.0f,  1.0f,  1.0f));
			osdMesh.vertices.Add(new Vector3( 1.0f,  1.0f, -1.0f));

			osdMesh.vertices.Add(new Vector3(-1.0f, -1.0f, -1.0f));
			osdMesh.vertices.Add(new Vector3(-1.0f, -1.0f,  1.0f));
			osdMesh.vertices.Add(new Vector3( 1.0f, -1.0f,  1.0f));
			osdMesh.vertices.Add(new Vector3( 1.0f, -1.0f, -1.0f));

			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
			osdMesh.indices.Add(0); osdMesh.indices.Add(1); osdMesh.indices.Add(2); osdMesh.indices.Add(3);

//			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
//			osdMesh.indices.Add(4); osdMesh.indices.Add(5); osdMesh.indices.Add(6); osdMesh.indices.Add(7);

			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
			osdMesh.indices.Add(0); osdMesh.indices.Add(1); osdMesh.indices.Add(5); osdMesh.indices.Add(4);

//			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
//			osdMesh.indices.Add(2); osdMesh.indices.Add(3); osdMesh.indices.Add(7); osdMesh.indices.Add(6);
//
//			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
//			osdMesh.indices.Add(0); osdMesh.indices.Add(1); osdMesh.indices.Add(2); osdMesh.indices.Add(3);
//
//			osdMesh.faces.Add(new OpenSubdiv.Face { startIndex = osdMesh.indices.Count, vertexCount = 4 });
//			osdMesh.indices.Add(0); osdMesh.indices.Add(1); osdMesh.indices.Add(2); osdMesh.indices.Add(3);
		}

		osdMesh.subdivisionLevel = 1;
		osdSubdividedMesh = osdMesh.Subdiv();
	}

	void LoadSubdividedMesh() {
		Mesh m = new Mesh();
		m.vertices = osdSubdividedMesh.vertices;
		m.RecalculateNormals();
		m.SetTriangles(osdSubdividedMesh.triangles, 0);
		GetMeshFilter().mesh = m;
	}
	
	void Start () {
		LoadOBJMesh();
		TestOpenSubdiv();
	}

	public void ButtonClick1() {
		LoadSubdividedMesh();
	}

	public void ButtonClick2() {
		LoadOBJMesh();
	}
}
