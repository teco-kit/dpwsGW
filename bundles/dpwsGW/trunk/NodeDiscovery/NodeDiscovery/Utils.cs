using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NodeDiscovery
{
    public static class Utils
    {
        /// <summary>
        /// Start a new process.
        /// </summary>
        /// <param name="file">The executable file to start.</param>
        /// <param name="arguments">Additional arguments.</param>
        /// <returns>The new process instance.</returns>
        public static System.Diagnostics.Process StartNewProcess(string file, string arguments)
        {
            if (!System.IO.File.Exists(file))
                throw new System.IO.FileNotFoundException(string.Format("Could not find the executable file: {0}. Please, review the Service settings.", file));

            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = file;
            proc.StartInfo.Arguments = arguments;
            proc.StartInfo.CreateNoWindow = false;
            proc.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Minimized;
            proc.Start();

            return proc;
        }

        /// <summary>
        /// Returns an Object from the XML.
        /// </summary>
        /// <param name="xml">The XML.</param>
        /// <param name="t">The type.</param>
        /// <param name="defaultNamespace">The default namespace.</param>
        /// <returns>The object.</returns>
        public static object FromXML(string xml, Type t, string defaultNamespace)
        {
            return FromXML(xml, t, defaultNamespace, null);
        }

        /// <summary>
        /// Returns an Object from the XML.
        /// </summary>
        /// <param name="xml">The XML.</param>
        /// <param name="t">The type.</param>
        /// <param name="defaultNamespace">The default namespace.</param>
        /// <param name="root">The root.</param>
        /// <returns>The object.</returns>
        public static object FromXML(string xml, Type t, string defaultNamespace, System.Xml.Serialization.XmlRootAttribute root)
        {
            if (string.IsNullOrEmpty(xml))
                return null;

            System.Xml.Serialization.XmlSerializer serializer;
            if (root != null)
                serializer = new System.Xml.Serialization.XmlSerializer(t, null, null, root, defaultNamespace);
            else
                serializer = new System.Xml.Serialization.XmlSerializer(t, defaultNamespace);

            try
            {
                return serializer.Deserialize(new System.IO.StringReader(xml));
            }
            catch (System.Xml.XmlException) { }

            return null;
        }
    }
}
