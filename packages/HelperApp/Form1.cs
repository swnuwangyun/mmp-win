using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HelperApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string[] lines = this.richTextBox1.Text.Split('\n');
            int offset = 3;

            string text = "";
            string code = "std::vector<std::map<std::wstring, glm::vec3>> bodyInfoList = {\r\n";
            string temp = "";

            while (true)
            {
                string[] items;

                string name = lines[offset++];
                items = name.Split('\"');
                name = items[1];

                string x = lines[offset++];
                items = x.Split('\"');
                x = items[3];

                string y = lines[offset++];
                items = y.Split('\"');
                y = items[3];

                string z = lines[offset++];
                items = z.Split('\"');
                z = items[3];

                temp += string.Format("     {{ L\"{0}\", glm::vec3({1}, {2}, {3}) }},\r\n", name, x, y, z);
                text += string.Format("{0}, {1}, {2}, {3}\r\n", name, x, y, z);

                if (name=="SpineBase")
                {
                    // Begin
                }
                if (name=="ThumbRight")
                {
                    // End
                    code += "   {\r\n";
                    temp = temp.Substring(0, temp.Length - 3) + "\r\n";
                    code += temp;
                    code += "   },\r\n";
                    temp = "";
                    text += "\r\n";

                    offset += 4;
                }
                offset++;

                if (offset > lines.Length)
                {
                    code = code.Substring(0, code.Length - 3) + "\r\n";
                    code += "};\r\n";
                    break;
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string[] lines = this.richTextBox2.Text.Split('\n');
            int offset = 1;

            string code = "std::map<std::wstring, KinectListItem> kinectBoneList = {\r\n";
            string temp = "";

            while (true)
            {
                string[] items;

                string kinect = lines[offset++];
                items = kinect.Split(':');
                kinect = items[0].Trim();

                string name = lines[offset++];
                items = name.Split('\"');
                name = items[1];

                string child = lines[offset++];
                items = child.Split('\"');
                if (items.Length > 2)
                    child = items[1];
                else
                {
                    child = "";
                    offset--;
                }

                temp += string.Format(" {{ L\"{0}\", {{ L\"{1}\", L\"{2}\", {3} }} }},\r\n", kinect, name, child, -1);
                if (child == "")
                    temp += "\r\n";

                if (kinect == "SpineMid")
                {
                    // Begin
                }
                if (kinect == "FootRight")
                {
                    // End
                    temp = temp.Substring(0, temp.Length - 2);
                    temp = temp.Substring(0, temp.Length - 3) + "\r\n";
                    code += temp;
                    code += "};\r\n";
                    break;
                }
                offset++;
            }
        }
    }
}
