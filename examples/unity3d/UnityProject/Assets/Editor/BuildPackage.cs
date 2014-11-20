
using System.IO;

using UnityEngine;
using UnityEditor;

public class BuildPackage 
{
	public static string[] m_PackageInputPaths = 
	{
		"Assets/OpenSubdiv",
		"Assets/Plugins"
	};

	public const string m_PackageOutputPath = "../OpenSubdiv.unitypackage";

	[MenuItem("Build/OpenSubdiv/Build Package")]
	public static void BuildUnityPackage()
	{
		AssetDatabase.ExportPackage(m_PackageInputPaths, m_PackageOutputPath, ExportPackageOptions.Recurse);
	}
}
