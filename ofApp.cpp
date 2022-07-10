#include "ofApp.h"	

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(255);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	this->cap.open("D:\\mp4\\MOV_0324.mp4");
	this->cap_size = cv::Size(128, 72);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->mat = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);// -skip_frame;
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->mat_list.push_back(tmp);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	this->mesh_list.clear();
	this->line_list.clear();

	float R = 300;
	int v_span = 18;
	int u_span = 36;

	for (int v = 0; v < 360; v += v_span) {

		for (auto u = 0; u < 360; u += u_span) {

			ofMesh mesh, line;
			line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

			auto noise_seed = ofRandom(1000);
			auto r = 80.f;
			auto noise_location = this->make_point(R, 90, u + u_span * 0.5, v);
			auto noise_value = ofNoise(noise_location.x * 0.001, noise_location.z * 0.001, noise_location.y * 0.002 + ofGetFrameNum() * 0.02);
			if (noise_value > 0.5) { 

				auto random_noise_value = ofNoise(noise_seed, ofGetFrameNum() * 0.005);
				r = 80.f + ofMap(random_noise_value, 0, 1, 0, 100) * ofMap(noise_value, 0.5, 1, 0, 1);
			}

			vector<glm::vec3> vertices;
			vertices.push_back(this->make_point(R, r, u, v - v_span * 0.5));
			vertices.push_back(this->make_point(R, r, u + u_span, v - v_span * 0.5));
			vertices.push_back(this->make_point(R, r, u + u_span, v + v_span * 0.5));
			vertices.push_back(this->make_point(R, r, u, v + v_span * 0.5));

			mesh.addVertices(vertices);
			
			mesh.addTexCoord(glm::vec3(this->cap_size.width, 0, 0));
			mesh.addTexCoord(glm::vec3(this->cap_size.width, this->cap_size.height, 0));
			mesh.addTexCoord(glm::vec3(0, this->cap_size.height, 0));
			mesh.addTexCoord(glm::vec3(0, 0, 0));
			
			mesh.addIndex(0); mesh.addIndex(1); mesh.addIndex(2);
			mesh.addIndex(0); mesh.addIndex(2); mesh.addIndex(3);

			line.addVertices(vertices);

			line.addIndex(0); line.addIndex(1);
			line.addIndex(1); line.addIndex(2);
			line.addIndex(2); line.addIndex(3);
			line.addIndex(3); line.addIndex(0);

			this->mesh_list.push_back(mesh);
			this->line_list.push_back(line);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateZ(ofGetFrameNum() * 0.22222);

	for (int i = 0; i < this->mesh_list.size(); i++) {

		ofSetColor(255);
		ofFill();
		this->mat_list[(int)(ofRandom(1000) + ofGetFrameNum()) % this->mat_list.size()].copyTo(this->mat);
		this->image.update();

		this->image.bind();
		this->mesh_list[i].draw();
		this->image.unbind();

		ofSetColor(255);
		ofNoFill();
		this->line_list[i].drawWireframe();
	}

	this->cam.end();
}

//--------------------------------------------------------------
glm::vec3 ofApp::make_point(float R, float r, float u, float v) {

	// 数学デッサン教室 描いて楽しむ数学たち　P.31

	u *= DEG_TO_RAD;
	v *= DEG_TO_RAD;

	auto x = (R + r * cos(u)) * cos(v);
	auto y = (R + r * cos(u)) * sin(v);
	auto z = r * sin(u);

	return glm::vec3(x, y, z);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}