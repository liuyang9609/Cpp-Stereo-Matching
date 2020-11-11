#include "Evision3dViz.h"
#include <QMessageBox>
#include <QFileDialog>
#include <qcolordialog.h>
#include <QAction>
#include <QMenu>
#include <QPoint>
#include <QDebug>
#include <vtkOutputWindow.h>
#include <vtkObject.h>
//���캯��
Evision3dViz::Evision3dViz(QWidget *parent)
	: QWidget(parent)
{
	//vtkOutputWindow::SetGlobalWarningDisplay(false);
	//vtkMapper::SetGlobalWarningDisplay(false);
	//vtkRenderWindow::SetGlobalWarningDisplay(false);
	//vtkObject::GlobalWarningDisplayOff();
	ui.setupUi(this);
	vtkObject::GlobalWarningDisplayOff();

	initial();// Initialization
	//vtkSmartPointer<vtkOutputWindow> myOutputWindow = vtkSmartPointer<vtkOutputWindow>::New();
	//myOutputWindow->PromptUserOff();
	//myOutputWindow->SetGlobalWarningDisplay(false);
	//vtkOutputWindow::SetInstance(myOutputWindow);
}

//��
void Evision3dViz::onPushButton_Open()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply *.obj);;All file(*.*)"));
	//Return if filenames is empty
	if (filenames.isEmpty())
		return;

	// Clear cache
	mycloud_vec.clear();
	total_points = 0;
	ui.dataTree->clear();
	viewer->removeAllPointClouds();

	// Open point cloud one by one
	for (int i = 0; i != filenames.size(); i++) {
		// time start
		timeStart();
		mycloud.cloud.reset(new PointCloudT); // Reset cloud
		QString filename = filenames[i];
		std::string file_name = filename.toStdString();
		std::string subname = getFileName(file_name);  //��ȡȫ·���е��ļ���������׺��

		//����״̬��
		//ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + " point cloud loading...");

		int status = -1;
		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else
		{
			//��ʾ���޷���ȡ����.ply .pcd .obj������ļ�
			QMessageBox::information(this, tr("File format error"),
				tr("Can't open files except .ply .pcd .obj"));
			return;
		}
		//��ʾ����׺û���⣬���ļ������޷���ȡ
		if (status != 0)
		{
			QMessageBox::critical(this, tr("Reading file error"), tr("We can not open the file"));
			return;
		}
		setA(255);  //���õ���Ϊ��͸��
		// �����ĵ��Ƶ���Ϣ
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //�����Ƶ����������


		// time off
		time_cost = timeOff();
		// �������
		consoleLog("Open", QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str()), "Time cost: " + time_cost + " s, Points: " + QString::number(mycloud.cloud->points.size()));

		//������Դ������
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList()
			<< QString::fromLocal8Bit(subname.c_str()));
		cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
		ui.dataTree->addTopLevelItem(cloudName);

		//setWindowTitle(filename + " - CloudViewer"); //���±���

		total_points += mycloud.cloud->points.size();
	}
	//ui.statusBar->showMessage("");
	showPointcloudAdd();  //������ͼ����
	setPropertyTable();
}
//���
void Evision3dViz::onPushButton_Add()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply *.obj);;All file(*.*)"));
	if (filenames.isEmpty())
		return;
	for (int i = 0; i != filenames.size(); i++) {
		// time start
		timeStart();
		mycloud.cloud.reset(new PointCloudT);
		QString filename = filenames[i];
		std::string file_name = filename.toStdString();
		std::string subname = getFileName(file_name);

		// ����״̬��
		//ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + " point cloud loading...");

		int status = -1;
		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else
		{
			//��ʾ���޷���ȡ����.ply .pcd .obj������ļ�
			QMessageBox::information(this, tr("File format error"), tr("Can't open files except .ply .pcd .obj"));
			return;
		}
		//��ʾ����׺û���⣬���ļ������޷���ȡ
		if (status != 0)
		{
			QMessageBox::critical(this, tr("Reading file error"), tr("We can not open the file"));
			return;
		}
		setA(255);  //���õ���Ϊ��͸��
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //�����Ƶ����������

		// time of
		time_cost = timeOff();
		//�������
		consoleLog("Add", QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str()), "Time cost: " + time_cost + " s, Points: " + QString::number(mycloud.cloud->points.size()));

		//������Դ������
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit(subname.c_str()));
		cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
		ui.dataTree->addTopLevelItem(cloudName);

		//setWindowTitle("CloudViewer");
		total_points += mycloud.cloud->points.size();
	}
	//ui.statusBar->showMessage("");
	showPointcloudAdd();
	setPropertyTable();
}
//���
void Evision3dViz::onPushButton_Clean()
{
	mycloud_vec.clear();  //�ӵ����������Ƴ����е���
	viewer->removeAllPointClouds();  //��viewer���Ƴ����е���
	viewer->removeAllShapes(); //���remove������
	ui.dataTree->clear();  //��dataTree���

	ui.propertyTable->clear();  //������Դ���propertyTable
	QStringList header;
	header << "Property" << "Value";
	ui.propertyTable->setHorizontalHeaderLabels(header);

	//�������
	consoleLog("Clear", "All point clouds", "", "");

	setWindowTitle("CloudViewer");  //���´��ڱ���
	showPointcloud();  //������ʾ
}
//����
void Evision3dViz::onPushButton_Save()
{
	save_filename = QFileDialog::getSaveFileName(this, tr("Save point cloud"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply);;Allfile(*.*)"));
	std::string file_name = save_filename.toStdString();
	std::string subname = getFileName(file_name);
	//�ļ���Ϊ��ֱ�ӷ���
	if (save_filename.isEmpty())
		return;

	if (mycloud_vec.size() > 1)
	{
		savemulti();
		return;
	}

	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		status = pcl::io::savePCDFile(file_name, *(mycloud.cloud));
	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		status = pcl::io::savePLYFile(file_name, *(mycloud.cloud));
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"),
			tr("Can't save files except .ply .pcd"));
		return;
	}
	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"),
			tr("We can not save the file"));
		return;
	}

	//�������
	consoleLog("Save", QString::fromLocal8Bit(subname.c_str()), save_filename, "Single save");

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"),
		QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}
//���Ϊ
void Evision3dViz::onPushButton_SaveAs()
{
	save_filename = QFileDialog::getSaveFileName(this, tr("Save point cloud as binary file"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply);;Allfile(*.*)"));
	std::string file_name = save_filename.toStdString();
	std::string subname = getFileName(file_name);
	//�ļ���Ϊ��ֱ�ӷ���
	if (save_filename.isEmpty())
		return;

	if (mycloud_vec.size() > 1)
	{
		savemulti();
		return;
	}

	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		status = pcl::io::savePCDFileBinary(file_name, *(mycloud.cloud));
	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		status = pcl::io::savePLYFileBinary(file_name, *(mycloud.cloud));
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"),
			tr("Can't save files except .ply .pcd"));
		return;
	}
	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"),
			tr("We can not save the file"));
		return;
	}

	//�������
	consoleLog("Save as binary", QString::fromLocal8Bit(subname.c_str()), save_filename, "Single save (binary)");

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"),
		QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}
//����(��ʽת��)
void Evision3dViz::onPushButton_Change()
{
	//TODO::���Ƹ�ʽת��
}
//�˳�
void Evision3dViz::onPushButton_Exit()
{
	this->close();
}
//������ɫ
void Evision3dViz::onPushButton_PointColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this, "Select color for point cloud");

	if (color.isValid()) //�ж���ѡ����ɫ�Ƿ���Ч
	{
		//QAction* action = dynamic_cast<QAction*>(sender());
		//if (action != ui.pointcolorAction) //�ı���ɫ���ź������� dataTree
		QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
		int selected_item_count = ui.dataTree->selectedItems().size();
		if (selected_item_count == 0) {
			for (int i = 0; i != mycloud_vec.size(); i++) {
				for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
					mycloud_vec[i].cloud->points[j].r = color.red();
					mycloud_vec[i].cloud->points[j].g = color.green();
					mycloud_vec[i].cloud->points[j].b = color.blue();
				}
			}
			// �������
			consoleLog("Change cloud color", "All point clouds", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		}
		else {
			for (int i = 0; i != selected_item_count; i++) {
				int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
				for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
					mycloud_vec[cloud_id].cloud->points[j].r = color.red();
					mycloud_vec[cloud_id].cloud->points[j].g = color.green();
					mycloud_vec[cloud_id].cloud->points[j].b = color.blue();
				}
			}
			// �������
			consoleLog("Change cloud color", "Point clouds selected", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		}
		//��ɫ�ĸı�ͬ����RGBͣ������
		ui.rSlider->setValue(color.red());
		ui.gSlider->setValue(color.green());
		ui.bSlider->setValue(color.blue());

		showPointcloud();
	}
}
//������ɫ
void Evision3dViz::onPushButton_BackgroundColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this,
		"Select color for point cloud");
	if (color.isValid())
	{
		viewer->setBackgroundColor(color.red() / 255.0,
			color.green() / 255.0, color.blue() / 255.0);
		// �������
		consoleLog("Change bg color", "Background", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		showPointcloud();
	}
}
//����ͼ
void Evision3dViz::onPushButton_MainView()
{
	viewer->setCameraPosition(0, -1, 0, 0.5, 0.5, 0.5, 0, 0, 1);
	ui.screen->update();
}
//����ͼ
void Evision3dViz::onPushButton_LeftView()
{
	viewer->setCameraPosition(-1, 0, 0, 0, 0, 0, 0, 0, 1);
	ui.screen->update();
}
//����ͼ
void Evision3dViz::onPushButton_TopViewe()
{
	viewer->setCameraPosition(0, 0, 1, 0, 0, 0, 0, 1, 0);
	ui.screen->update();
}
//����cube
void Evision3dViz::onPushButton_GenCube()
{
	mycloud.cloud.reset(new PointCloudT);
	total_points = 0;
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllPointClouds();  //��viewer���Ƴ����е���
	mycloud_vec.clear();  //��յ�������

	mycloud.cloud->width = 50000;         // ���õ��ƿ�
	mycloud.cloud->height = 1;            // ���õ��Ƹߣ���Ϊ1��˵��Ϊ����֯����
	mycloud.cloud->is_dense = false;
	mycloud.cloud->resize(mycloud.cloud->width * mycloud.cloud->height);     // ���õ��ƴ�С
	for (int i = 0; i != mycloud.cloud->size(); ++i)
	{
		mycloud.cloud->points[i].x = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].y = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].z = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].r = red;
		mycloud.cloud->points[i].g = green;
		mycloud.cloud->points[i].b = blue;
	}
	//������Դ������
	QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("cube"));
	cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
	ui.dataTree->addTopLevelItem(cloudName);

	// �������
	consoleLog("Generate cube", "cube", "cube", "");

	mycloud_vec.push_back(mycloud);
	showPointcloudAdd();
}
//����sphere
void Evision3dViz::onPushButton_GenSphere()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //��յ�������

	pcl::PointXYZ p;
	p.x = 0; p.y = 0; p.z = 0;
	viewer->addSphere(p, 100, "sphere1");

	viewer->resetCamera();
	ui.screen->update();

	// �������
	consoleLog("Create sphere", "Sphere", "", "Succeeded");
}
//����Cylinder
void Evision3dViz::onPushButton_GenCylinder()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //��յ�������

	viewer->addCylinder(*(new pcl::ModelCoefficients()), "cylinder");

	viewer->resetCamera();
	ui.screen->update();

	// �������
	consoleLog("Create cylinder", "Cylinder", "", "Failed");
}
//MeshSurface���߹��ơ������ؽ���������Ƭ��ʾ
void Evision3dViz::onPushButton_MeshSurface()
{
	/* ����
	����÷���ֻ�ܴ���PointXYZ�ĵ��ƣ���PointXZYRGBA�ĵ��Ʊ���ᱨ��
	����boost::this_thread::sleep����Ҳ��������
	*/
	pcl::PointXYZ point;
	cloud_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);
	for (int i = 0; i < mycloud.cloud->size(); i++)
	{
		point.x = mycloud.cloud->points[i].x;
		point.y = mycloud.cloud->points[i].y;
		point.z = mycloud.cloud->points[i].z;
		cloud_xyz->push_back(point);
	}
	if (!cloud_xyz)
	{
		return;//ʧ���˳�
	}

	/****** �������ģ�� ******/
	//�������߹��ƶ��� n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//������������ָ�� normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//���� kdtree ���ڷ������ʱ��������
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //Ϊ kdtree �������
	n.setInputCloud(cloud_xyz); //Ϊ������ƶ����������
	n.setSearchMethod(tree);  //���÷������ʱ����ȡ��������ʽΪkdtree
	n.setKSearch(20); //���÷������ʱ��k���������ĵ���
	n.compute(*normals); //���з������

	QMessageBox::information(this, "information", "Normal estimation finished");

	/****** ���������뷨������ƴ�� ******/
	//����֮���������

	//����ͬʱ������ͷ��ߵ����ݽṹ��ָ��
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//���ѻ�õĵ����ݺͷ�������ƴ��
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //������������cloud�������йأ��ĳ�PointXYZ�ĵ��ƾ�û�б�����

	//������һ��kdtree�����ؽ�
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//Ϊkdtree����������ݣ��õ�����������Ϊ��ͷ���
	tree2->setInputCloud(cloud_with_normals);

	/****** �����ؽ�ģ�� ******/
	//����̰��������ͶӰ�ؽ�����
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//���������������������洢�ؽ����
	pcl::PolygonMesh triangles;
	//���ò���
	gp3.setSearchRadius(25); //�������ӵ�֮�������룬����ȷ��k���ڵ���뾶
	gp3.setMu(2.5); //��������ھ���ĳ��ӣ��Եõ�ÿ��������������뾶
	gp3.setMaximumNearestNeighbors(100); //��������������ڵ���������
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45�� ���ƽ���
	gp3.setMinimumAngle(M_PI / 18); //10�� ÿ�����ǵ����Ƕȣ�
	gp3.setMaximumAngle(2 * M_PI / 3); //120��
	gp3.setNormalConsistency(false); //��������һ�£���Ϊtrue
	//���õ������ݺ�������ʽ
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// ��ʼ�ؽ�
	gp3.reconstruct(triangles);
	QMessageBox::information(this, "informaiton", "Reconstruction finished");

	//���ؽ�������浽Ӳ���ļ��У��ؽ������VTK��ʽ�洢
	pcl::io::saveVTKFile("mymesh.vtk", triangles);

	/*
	//Additional vertex information
	std::vector<int> parts = gp3.getPartIDs();
	std::vector<int> states = gp3.getPointStates();
	fstream fs;
	fs.open("partsID.txt", ios::out);
	if (!fs)
	{
	return -2;
	}
	fs << "number of point clouds:" << parts.size() << "\n";
	for (int i = 0; i < parts.size(); i++)
	{
	if (parts[i] != 0)
	{
	fs << parts[i] << "\n";
	}
	}
	*/

	/****** ͼ����ʾģ�� ******/
	QMessageBox::information(this, "informaiton", "Start to show");
	viewer->addPolygonMesh(triangles, "my"); //����Ҫ��ʾ���������
	//��������ģ����ʾģʽ
	viewer->setRepresentationToSurfaceForAllActors(); //����ģ������Ƭ��ʽ��ʾ
	//viewer->setRepresentationToPointsForAllActors(); //����ģ���Ե���ʽ��ʾ
	//viewer->setRepresentationToWireframeForAllActors(); //����ģ�����߿�ͼģʽ��ʾ

	// �������
	consoleLog("Convert surface", "", "", "");

	viewer->removeAllShapes();
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		//boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	return;
}
//Wireframe
void Evision3dViz::onPushButton_Wireframe()
{
	/* ����
����÷���ֻ�ܴ���PointXYZ�ĵ��ƣ���PointXZYRGBA�ĵ��Ʊ���ᱨ��
����boost::this_thread::sleep����Ҳ��������
*/
	pcl::PointXYZ point;
	cloud_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);
	for (int i = 0; i < mycloud.cloud->size(); i++)
	{
		point.x = mycloud.cloud->points[i].x;
		point.y = mycloud.cloud->points[i].y;
		point.z = mycloud.cloud->points[i].z;
		cloud_xyz->push_back(point);
	}
	if (!cloud_xyz)
	{
		return;
	}


	/****** �������ģ�� ******/
	//�������߹��ƶ��� n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//������������ָ�� normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//���� kdtree ���ڷ������ʱ��������
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //Ϊ kdtree �������
	n.setInputCloud(cloud_xyz); //Ϊ������ƶ����������
	n.setSearchMethod(tree);  //���÷������ʱ����ȡ��������ʽΪkdtree
	n.setKSearch(20); //���÷������ʱ��k���������ĵ���
	n.compute(*normals); //���з������

	QMessageBox::information(this, "information", "Normal estimation finished");

	/****** ���������뷨������ƴ�� ******/
	//����֮���������

	//����ͬʱ������ͷ��ߵ����ݽṹ��ָ��
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//���ѻ�õĵ����ݺͷ�������ƴ��
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //������������cloud�������йأ��ĳ�PointXYZ�ĵ��ƾ�û�б�����

	//������һ��kdtree�����ؽ�
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//Ϊkdtree����������ݣ��õ�����������Ϊ��ͷ���
	tree2->setInputCloud(cloud_with_normals);



	/****** �����ؽ�ģ�� ******/
	//����̰��������ͶӰ�ؽ�����
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//���������������������洢�ؽ����
	pcl::PolygonMesh triangles;
	//���ò���
	gp3.setSearchRadius(25); //�������ӵ�֮�������룬����ȷ��k���ڵ���뾶
	gp3.setMu(2.5); //��������ھ���ĳ��ӣ��Եõ�ÿ��������������뾶
	gp3.setMaximumNearestNeighbors(100); //��������������ڵ���������
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45�� ���ƽ���
	gp3.setMinimumAngle(M_PI / 18); //10�� ÿ�����ǵ����Ƕȣ�
	gp3.setMaximumAngle(2 * M_PI / 3); //120��
	gp3.setNormalConsistency(false); //��������һ�£���Ϊtrue
	//���õ������ݺ�������ʽ
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// ��ʼ�ؽ�
	gp3.reconstruct(triangles);
	QMessageBox::information(this, "informaiton", "Reconstruction finished");

	//���ؽ�������浽Ӳ���ļ��У��ؽ������VTK��ʽ�洢
	pcl::io::saveVTKFile("mymesh.vtk", triangles);

	/*
	//Additional vertex information
	std::vector<int> parts = gp3.getPartIDs();
	std::vector<int> states = gp3.getPointStates();
	fstream fs;
	fs.open("partsID.txt", ios::out);
	if (!fs)
	{
	return -2;
	}
	fs << "number of point clouds:" << parts.size() << "\n";
	for (int i = 0; i < parts.size(); i++)
	{
	if (parts[i] != 0)
	{
	fs << parts[i] << "\n";
	}
	}
	*/

	/****** ͼ����ʾģ�� ******/
	QMessageBox::information(this, "informaiton", "Start to show");
	viewer->addPolygonMesh(triangles, "my"); //����Ҫ��ʾ���������
	//��������ģ����ʾģʽ
	//viewer->setRepresentationToSurfaceForAllActors(); //����ģ������Ƭ��ʽ��ʾ
	//viewer->setRepresentationToPointsForAllActors(); //����ģ���Ե���ʽ��ʾ
	viewer->setRepresentationToWireframeForAllActors(); //����ģ�����߿�ͼģʽ��ʾ

	// �������
	consoleLog("Convert wireframe", "", "", "");

	viewer->removeAllShapes();
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		//boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	return;
}
//�����ɫ
void Evision3dViz::onPushButton_RandomColor()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// ���δѡ���κε��ƣ������ͼ�����е����е��ƽ�����ɫ
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].g = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].b = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
			}
		}

		// �������
		consoleLog("Random color", "All point clous", "", "");

	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
				mycloud_vec[cloud_id].cloud->points[j].r = red;
				mycloud_vec[cloud_id].cloud->points[j].g = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[cloud_id].cloud->points[j].b = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
			}
		}

		// �������
		consoleLog("Random color", "Point clouds selected", "", "");
	}
	showPointcloud();
}
//R Slider ֵ����
void Evision3dViz::onValueChanged_r_Slider(int value)
{
	red = value;
	ui.rLCD->display(value);
}
//G Slider ֵ����
void Evision3dViz::onValueChanged_g_Slider(int value)
{
	green = value;
	ui.gLCD->display(value);
}
//B Slider ֵ����
void Evision3dViz::onValueChanged_b_Slider(int value)
{
	blue = value;
	ui.bLCD->display(value);
}
//P Slider ֵ����
void Evision3dViz::onValueChanged_p_Slider(int value)
{
	p = value;
	ui.sizeLCD->display(value);
}
//RGB Slider �ͷ�
void Evision3dViz::onReleased_rgb_Slider()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// ���δѡ���κε��ƣ������ͼ�����е����е��ƽ�����ɫ
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = red;
				mycloud_vec[i].cloud->points[j].g = green;
				mycloud_vec[i].cloud->points[j].b = blue;
			}
		}

		// �������
		consoleLog("Change cloud color", "All point clouds", QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
				mycloud_vec[cloud_id].cloud->points[j].r = red;
				mycloud_vec[cloud_id].cloud->points[j].g = green;
				mycloud_vec[cloud_id].cloud->points[j].b = blue;
			}
		}
		// �������
		consoleLog("Change cloud color", "Point clouds selected", QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
	}
	showPointcloud();
}
//P Slider �ͷ�
void Evision3dViz::onReleased_p_Slider()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + QString::number(i).toStdString());
		}
		// �������
		consoleLog("Change cloud size", "All point clouds", "Size: " + QString::number(p), "");
	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + QString::number(cloud_id).toStdString());
		}
		// �������
		consoleLog("Change cloud size", "Point clouds selected", "Size: " + QString::number(p), "");
	}
	ui.screen->update();
}
//checkbox Coordinate ״̬����
void Evision3dViz::onCheckboxStateChanged_Coordinate(int value)
{
	switch (value)
	{
	case 0:
		viewer->removeCoordinateSystem();  //�Ƴ�����ϵ
		// �������
		consoleLog("Remove coordinate system", "Remove", "", "");
		break;
	case 2:
		viewer->addCoordinateSystem();  //�������ϵ
		// �������
		consoleLog("Add coordinate system", "Add", "", "");
		break;
	}
	//viewer->updatePointCloud(cloud, "cloud");
	ui.screen->update();
}
//checkbox DarkLight ״̬����
void Evision3dViz::onCheckboxStateChanged_DarkLight(int value)
{
	switch (value)
	{
	case 0:
		viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);
		// �������
		consoleLog("Change bg color", "Background", "30 30 30", "");
		break;
	case 2:
		//��ע�⣺setBackgroundColor()���յ���0-1��double�Ͳ���
		viewer->setBackgroundColor(240 / 255.0, 240 / 255.0, 240 / 255.0);
		// �������
		consoleLog("Change bg color", "Background", "240 240 240", "");
		break;
	}
	//viewer->updatePointCloud(cloud, "cloud");
	ui.screen->update();
}
//���ƶ���ѡ��
void Evision3dViz::itemSelected(QTreeWidgetItem* item, int count)
{
	count = ui.dataTree->indexOfTopLevelItem(item);  //��ȡitem���к�

	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud" + QString::number(i).toStdString());
	}

	//��ȡ��ǰ���Ƶ�RGB,������������Ϣ
	int cloud_size = mycloud_vec[count].cloud->points.size();
	unsigned int cloud_r = mycloud_vec[count].cloud->points[0].r;
	unsigned int cloud_g = mycloud_vec[count].cloud->points[0].g;
	unsigned int cloud_b = mycloud_vec[count].cloud->points[0].b;
	bool multi_color = true;
	if (mycloud_vec[count].cloud->points.begin()->r == (mycloud_vec[count].cloud->points.end() - 1)->r) //�жϵ��Ƶ�ɫ��ɫ�����������Ǻ��Ͻ���
		multi_color = false;

	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(QString::number(cloud_size)));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(3, 1, new QTableWidgetItem(multi_color ? "Multi Color" : (QString::number(cloud_r) + " " + QString::number(cloud_g) + " " + QString::number(cloud_b))));

	//ѡ��item����Ӧ�ĵ��Ƴߴ���
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
			2, "cloud" + QString::number(cloud_id).toStdString());
	}
	//mycloud = mycloud_vec[count];
	ui.screen->update();
}
//�����Ҽ��˵�
void Evision3dViz::popMenu(const QPoint& point)
{
	QTreeWidgetItem* curItem = ui.dataTree->currentItem(); //��ȡ��ǰ������Ľڵ�
	if (curItem == NULL)return;           //����������Ҽ���λ�ò���treeItem�ķ�Χ�ڣ����ڿհ�λ���һ�
	QString name = curItem->text(0);
	int id = ui.dataTree->indexOfTopLevelItem(curItem);
	std::string cloud_id = "cloud" + QString::number(id).toStdString();

	QAction hideItemAction("Hide", this);
	QAction showItemAction("Show", this);
	QAction deleteItemAction("Delete", this);
	QAction changeColorAction("Change color", this);

	connect(&hideItemAction, &QAction::triggered, this, &Evision3dViz::hideItem);
	connect(&showItemAction, &QAction::triggered, this, &Evision3dViz::showItem);
	connect(&deleteItemAction, &QAction::triggered, this, &Evision3dViz::deleteItem);
	connect(&changeColorAction, &QAction::triggered, this, &Evision3dViz::onPushButton_PointColor);

	QPoint pos;
	QMenu menu(ui.dataTree);
	menu.addAction(&hideItemAction);
	menu.addAction(&showItemAction);
	menu.addAction(&deleteItemAction);
	menu.addAction(&changeColorAction);

	if (mycloud_vec[id].visible == true) {
		menu.actions()[1]->setVisible(false);
		menu.actions()[0]->setVisible(true);
	}
	else {
		menu.actions()[1]->setVisible(true);
		menu.actions()[0]->setVisible(false);
	}


	menu.exec(QCursor::pos()); //�ڵ�ǰ���λ����ʾ
}
//�Ҽ��˵�:����
void Evision3dViz::hideItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//TODO hide֮��item��ɻ�ɫ���ٴ��һ�itemʱ����hideItem�� ѡ���� ��showItem��
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		std::string cloud_id = "cloud" + QString::number(id).toStdString();
		//QMessageBox::information(this, "cloud_id", QString::fromLocal8Bit(cloud_id.c_str()));
		// ��cloud_id����Ӧ�ĵ������ó�͸��
		viewer->setPointCloudRenderingProperties(pcl::visualization::RenderingProperties::PCL_VISUALIZER_OPACITY, 0.0, cloud_id, 0);
		QColor item_color = QColor(112, 122, 132, 255);
		curItem->setTextColor(0, item_color);
		mycloud_vec[id].visible = false;
	}

	// �������
	consoleLog("Hide point clouds", "Point clouds selected", "", "");

	ui.screen->update(); //ˢ����ͼ���ڣ�����ʡ��
}
//�Ҽ��˵�:��ʾ
void Evision3dViz::showItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		std::string cloud_id = "cloud" + QString::number(id).toStdString();
		// ��cloud_id����Ӧ�ĵ������ó�͸��
		viewer->setPointCloudRenderingProperties(pcl::visualization::RenderingProperties::PCL_VISUALIZER_OPACITY, 1.0, cloud_id, 0);
		QColor item_color;
		if (theme_id == 0) {
			item_color = QColor(0, 0, 0, 255);
		}
		else {
			item_color = QColor(241, 241, 241, 255);
		}
		curItem->setTextColor(0, item_color);
		mycloud_vec[id].visible = true;
	}

	// �������
	consoleLog("Show point clouds", "Point clouds selected", "", "");

	ui.screen->update(); //ˢ����ͼ���ڣ�����ʡ��
}
//�Ҽ��˵�:ɾ��
void Evision3dViz::deleteItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	// ui.dataTree->selectedItems().size() ���ŵ����������ı䣬���ѭ������Ҫ����Ϊ�̶���С�� selected_item_count
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		//QMessageBox::information(this, "itemList's size", QString::number(ui.dataTree->selectedItems().size()));
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		//QMessageBox::information(this, "information", "curItem: " + name + " " + QString::number(id));
		auto it = mycloud_vec.begin() + ui.dataTree->indexOfTopLevelItem(curItem);
		// ɾ������֮ǰ����������Ŀ����
		int delete_points = (*it).cloud->points.size();
		it = mycloud_vec.erase(it);
		//QMessageBox::information(this, "information", QString::number(delete_points) + " " + QString::number(mycloud_vec.size()));

		total_points -= delete_points;
		setPropertyTable();

		ui.dataTree->takeTopLevelItem(ui.dataTree->indexOfTopLevelItem(curItem));
	}

	// �Ƴ�֮������ӣ����� id ����Դ�������кŲ�һ�µ����
	viewer->removeAllPointClouds();
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}

	// �������
	consoleLog("Delete point clouds", "Point clouds selected", "", "");

	ui.screen->update();
}

//��ʼ��
void Evision3dViz::initial()
{
	//�����ʼ��
	setWindowIcon(QIcon(tr(":/Resources/images/icon.png")));
	setWindowTitle(tr("CloudViewer"));

	//���Ƴ�ʼ��
	mycloud.cloud.reset(new PointCloudT);
	mycloud.cloud->resize(1);
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//viewer->addPointCloud(cloud, "cloud");

	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	ui.screen->update();

	ui.propertyTable->setSelectionMode(QAbstractItemView::NoSelection); // ��ֹ������Թ������� item
	//ui.consoleTable->setSelectionMode(QAbstractItemView::NoSelection);  // ��ֹ���������ڵ� item
	ui.dataTree->setSelectionMode(QAbstractItemView::ExtendedSelection); // ���� dataTree ���ж�ѡ

	// ����Ĭ������
	//QString qss = darcula_qss;
	//qApp->setStyleSheet(qss);

	setPropertyTable();
	//setConsoleTable();

	// �������
	consoleLog("Software start", "CloudViewer", "Welcome to use CloudViewer", "Nightn");


	// ���ñ�����ɫΪ dark
	viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);
}
//���õ�����ɫ
void Evision3dViz::setCloudColor(unsigned r, unsigned g, unsigned b)
{
	// Set the new color
	for (int i = 0; i < mycloud.cloud->size(); i++)
	{
		mycloud.cloud->points[i].r = r;
		mycloud.cloud->points[i].g = g;
		mycloud.cloud->points[i].b = b;
		mycloud.cloud->points[i].a = 255;
	}
}
//����͸����
void Evision3dViz::setA(unsigned a)
{
	for (int i = 0; i < mycloud.cloud->size(); i++)
	{
		mycloud.cloud->points[i].a = a;
	}
}
//��ӵ��Ƶ�viewer,����ʾ����
void Evision3dViz::showPointcloudAdd()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}
	viewer->resetCamera();
	ui.screen->update();
}
//�������Թ�����
void Evision3dViz::setPropertyTable()
{
	QStringList header;
	header << "Property" << "Value";
	ui.propertyTable->setHorizontalHeaderLabels(header);
	ui.propertyTable->setItem(0, 0, new QTableWidgetItem("Clouds"));
	ui.propertyTable->setItem(1, 0, new QTableWidgetItem("Points"));
	ui.propertyTable->setItem(2, 0, new QTableWidgetItem("Total points"));
	ui.propertyTable->setItem(3, 0, new QTableWidgetItem("RGB"));


	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(""));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(4, 1, new QTableWidgetItem(""));
}
//��ʾ���ƣ�����������Ƕ�
void Evision3dViz::showPointcloud()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}
	//viewer->resetCamera();
	ui.screen->update();
}
// Save multi point cloud
void Evision3dViz::savemulti()
{
	std::string subname = getFileName(save_filename.toStdString());
	PointCloudT::Ptr multi_cloud;
	multi_cloud.reset(new PointCloudT);
	multi_cloud->height = 1;
	int sum = 0;
	for (auto c : mycloud_vec)
	{
		sum += c.cloud->points.size();
	}
	multi_cloud->width = sum;
	multi_cloud->resize(multi_cloud->height * multi_cloud->width);
	int k = 0;
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++)          //ע��cloudvec[i]->points.size()��cloudvec[i]->size()������
		{
			multi_cloud->points[k].x = mycloud_vec[i].cloud->points[j].x;
			multi_cloud->points[k].y = mycloud_vec[i].cloud->points[j].y;
			multi_cloud->points[k].z = mycloud_vec[i].cloud->points[j].z;
			multi_cloud->points[k].r = mycloud_vec[i].cloud->points[j].r;
			multi_cloud->points[k].g = mycloud_vec[i].cloud->points[j].g;
			multi_cloud->points[k].b = mycloud_vec[i].cloud->points[j].b;
			k++;
		}
	}
	//����multi_cloud
	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePCDFileBinary(save_filename.toStdString(), *multi_cloud);
		}
		else {
			status = pcl::io::savePCDFile(save_filename.toStdString(), *multi_cloud);
		}

	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePLYFileBinary(save_filename.toStdString(), *multi_cloud);
		}
		else {
			status = pcl::io::savePLYFile(save_filename.toStdString(), *multi_cloud);
		}
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"), tr("Can't save files except .ply .pcd"));
		return;
	}

	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"), tr("We can not save the file"));
		return;
	}

	// �������
	if (save_as_binary) {
		consoleLog("Save as binary", QString::fromLocal8Bit(subname.c_str()), save_filename, "Multi save (binary)");
	}
	else {
		consoleLog("Save", QString::fromLocal8Bit(subname.c_str()), save_filename, "Multi save");
	}


	save_as_binary = false;
	//�������� multi_cloud ����Ϊ��ǰ mycloud,�Ա㱣��֮��ֱ�ӽ��в���
	mycloud.cloud = multi_cloud;
	mycloud.filename = save_filename.toStdString();
	mycloud.subname = subname;

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"), QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}
//���������־
void Evision3dViz::consoleLog(QString operation, QString subname, QString filename, QString note)
{
	qDebug() << operation << subname << filename << note;
}
//��ȡȫ·���е��ļ���������׺��
std::string Evision3dViz::getFileName(std::string file_name)
{
	std::string subname;
	for (auto i = file_name.end() - 1; *i != '/'; i--)
	{
		subname.insert(subname.begin(), *i);
	}
	return subname;
}
//��ʱ��ʼ
void Evision3dViz::timeStart()
{
	time.start();
}
//��ʱ����
QString Evision3dViz::timeOff()
{
	int timediff = time.elapsed();   //���ش��ϴ�start()��restart()��ʼ������ʱ����λms
	float f = timediff / 1000.0;
	QString tr_timediff = QString("%1").arg(f);  //float->QString
	return tr_timediff;
}
