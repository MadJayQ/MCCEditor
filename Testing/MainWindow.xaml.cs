using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Path = System.IO.Path;
using Microsoft.Win32;
using HaloMCCEditor.Core.Blam;

namespace Testing
{   
    
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
        string fileName = null;
        private void btnOpenFile_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog(); // creates a open file dialog on click of button
            
            if (openFileDialog.ShowDialog() == true)
            {

                string getFileName = openFileDialog.FileName; // gets the file path as a string
                string fileExtension = Path.GetExtension(getFileName); //gets the extension of given file
                if (fileExtension != ".map") //checks the extension to make sure it's correct
                {
                    MessageBox.Show("Please select a valid .map file"); //error box to ask for a file with correct extension
                }
                else
                {
                    fileName = openFileDialog.FileName; //set the given files name to filename
                    BlamCacheFile blamCacheFile = new BlamCacheFile(fileName); //sends the file to HaloMCCEditor project
                }
                
            }
         
        }
        
    }

    
}
