using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using AOT;

namespace OpenSubdiv
{
	struct Face
	{
		public int startIndex;
		public int vertexCount;
	}

	class SubdividedMesh
	{
		public Vector3[] vertices;
		public int[] triangles;
	}

	class Mesh
	{
		public List<Vector3> vertices = new List<Vector3>();
		public List<int> indices = new List<int>();
		public List<Face> faces = new List<Face>();
		public int subdivisionLevel = 1;

		#if UNITY_IPHONE && !UNITY_EDITOR
		const string PLUGIN_NAME = "__Internal";
		#else
		const string PLUGIN_NAME = "OpenSubdivUnity";
		#endif

		[DllImport(PLUGIN_NAME)]
		public static extern void Initialize(bool enableCodegen);

		[DllImport(PLUGIN_NAME)]
		private static extern bool SetVertices(Vector3[] vertices, int vertexCount);
		
		[DllImport(PLUGIN_NAME)]
		private static extern bool SetFaces(int[] indicies, OpenSubdiv.Face[] faces, int faceCount);

		[DllImport(PLUGIN_NAME)]
		private static extern bool SetSubdivisionLevel(int level);
		
		[DllImport(PLUGIN_NAME)]
		private static extern bool Subdivide();

		protected delegate void UnityDebugLog(bool error, String text);
		
		[DllImport(PLUGIN_NAME)]
		private static extern void SetUnityDebugLog(UnityDebugLog unityDebugLog);

		[DllImport(PLUGIN_NAME)]
		private static extern void GetSubdividedVertices(ref IntPtr verices, ref int vertexCount);
		
		[DllImport(PLUGIN_NAME)]
		private static extern void GetSubdividedTriangles(ref IntPtr triangles, ref int triangleCount);

		protected SubdividedMesh LoadSubdividedMesh() {
			SubdividedMesh result = new SubdividedMesh();

			IntPtr pVertices = IntPtr.Zero;
			int vertexCount = 0;

			GetSubdividedVertices(ref pVertices, ref vertexCount);

			float[] vertices = new float[vertexCount];
			Marshal.Copy(pVertices, vertices, 0, vertexCount);

			result.vertices = new Vector3[vertexCount / 3];
			for (int i = 0; i < result.vertices.Length; i++) {
				result.vertices[i].x = vertices[3 * i + 0];
				result.vertices[i].y = vertices[3 * i + 1];
				result.vertices[i].z = vertices[3 * i + 2];
			}

			IntPtr pTriangles = IntPtr.Zero;
			int triangleCount = 0;

			GetSubdividedTriangles(ref pTriangles, ref triangleCount);

			result.triangles = new int[triangleCount];
			Marshal.Copy(pTriangles, result.triangles, 0, triangleCount);

			return result;
		}

		[MonoPInvokeCallback(typeof(UnityDebugLog))]
		private static void DebugLog(bool error, String text) {
			if (error) {
				Debug.LogError(text);
			} else {
				Debug.Log(text);
			}
		}
		 
		public SubdividedMesh Subdiv() {
			SetUnityDebugLog(DebugLog);

			if (!SetSubdivisionLevel(subdivisionLevel)) {
				return null;
			}

			if (!SetFaces(indices.ToArray(), faces.ToArray(), 2 * faces.Count)) {
				return null;
			}

			if (!SetVertices(vertices.ToArray(), 3 * vertices.Count)) {
				return null;
			}

			if (!Subdivide()) {
				return null;
			}

			return LoadSubdividedMesh();
		}
	}
}
