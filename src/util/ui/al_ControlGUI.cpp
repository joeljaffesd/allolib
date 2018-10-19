#include "al/util/ui/al_ControlGUI.hpp"

#include "al/util/ui/al_SequenceRecorder.hpp"

using namespace al;
using namespace std;

void ControlGUI::draw(Graphics &g) {
    auto separatorAnchor = mSeparatorAnchors.begin();
    auto groupBeginAnchor = mGroupBeginAnchors.begin();
    auto groupNamesIt = mGroupNames.begin();
    auto groupEndAnchor = mGroupEndAnchors.begin();


    if (mManageIMGUI) {
        begin();
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.69f, 0.00f, 0.53f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.80f, 0.69f, 0.00f, 0.53f);
    }
//    ImGui::SetNextWindowSize(ImVec2(300, 450), ImGuiCond_FirstUseEver);
//    ImGui::SetNextWindowPos(ImVec2(mX, mY), ImGuiCond_FirstUseEver);
//    ImGui::Begin(std::to_string(mId).c_str());
    if (mNav) { drawNav();}
    if (mPresetHandler) { drawPresetHandler(); }
    if (mPresetSequencer) {
        if (ImGui::CollapsingHeader("Preset Sequencer", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen )) {
            vector<string> seqList = mPresetSequencer->getSequenceList();
            if (seqList.size() > 0) {
                if (seqList.size() > 64) {
                    seqList.resize(64);
                    std::cout << "Cropping sequence list to 64 items for display" <<std::endl;
                }
                // for (size_t i = 0; i < seqList.size(); i++) {
                //     strncpy(mSequencerItems[i], seqList[i].c_str(), 32);
                // }
                // int items_count = seqList.size();

                ImGui::Combo("Sequences##PresetSequencer", &mCurrentPresetSequencerItem, vector_getter,
                            static_cast<void*>(&seqList), seqList.size());
                if (ImGui::Button("Play##PresetSequencer")) {
                    mPresetSequencer->playSequence(seqList[mCurrentSequencerItem]);
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop##PresetSequencer")) {
                    mPresetSequencer->stopSequence();
                }

                // for (size_t i = 0; i < seqList.size(); i++) {
                //     //                free(items[i]);
                // }
            }
        }
    }
    if (mSequenceRecorder) { drawSequenceRecorder(); }
    if (mPolySynth) {
        if (ImGui::CollapsingHeader("PolySynth", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Panic (All notes off)")) {
                mPolySynth->allNotesOff();
            }
        }
    }
    if (mSynthSequencer) {
        if (ImGui::CollapsingHeader("Event Sequencer##EventSequencer", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
            vector<string> seqList = mSynthSequencer->getSequenceList();
            if (seqList.size() > 0) {
                if (seqList.size() > 64) {
                    seqList.resize(64);
                    std::cout << "Cropping sequence list to 64 items for display" <<std::endl;
                }
                // for (size_t i = 0; i < seqList.size(); i++) {
                //     strncpy(mSequencerItems[i], seqList[i].c_str(), 32);
                // }
                // int items_count = seqList.size();
                ImGui::Combo("Sequences##EventSequencer", &mCurrentSequencerItem, vector_getter,
                            static_cast<void*>(&seqList), seqList.size());
                if (ImGui::Button("Play##EventSequencer")) {
                    mSynthSequencer->synth().allNotesOff();
                    mSynthSequencer->playSequence(seqList[mCurrentSequencerItem]);
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop##EventSequencer")) {
                    mSynthSequencer->stopSequence();
                    mSynthSequencer->synth().allNotesOff();
                }

                for (size_t i = 0; i < seqList.size(); i++) {
                    //                free(items[i]);
                }
            }
        }
    }
    if (mSynthRecorder) {
        if (ImGui::CollapsingHeader("Event Recorder##__EventRecorder", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
            static char buf1[64] = "test"; ImGui::InputText("Record Name##__EventRecorder", buf1, 64);
            if (ImGui::Checkbox("Record##__EventRecorder", &mRecordButtonValue)) {
                if (mRecordButtonValue) {
                    mSynthRecorder->startRecord(buf1, mOverwriteButtonValue);
                } else {
                    mSynthRecorder->stopRecord();
                }
            }
            ImGui::SameLine();
            ImGui::Checkbox("Overwrite", &mOverwriteButtonValue);
        }
    }

    for (auto bundleGroup: mBundles) {
        std::string suffix = "##_bundle_" + bundleGroup.first;
        drawBundleGroup(bundleGroup.second, suffix);
    }


//    ImGui::ShowDemoWindow();
    vector<bool> groupsVisibleStack;
    for (auto elem: mElements) {
        if(elem.first == "" || ImGui::CollapsingHeader(elem.first.c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) { // ! to force open by 
            string suffix;
            if (elem.first.size() > 0){
                suffix = "##" + elem.first;
            }

			for (ParameterMeta *p: elem.second) {
				// We do a runtime check to determine the type of the parameter to determine how to draw it.
                if (groupsVisibleStack.size() == 0 || groupsVisibleStack.back() == true) {
                    drawParameterMeta(p, suffix);
                    if (separatorAnchor != mSeparatorAnchors.end()) {
                        // The spacing's visibility depends on its position,
                        // So here we show it, but we need to do the increment
                        // below outside the visibility check
                        if (*separatorAnchor == p) {
                            ImGui::Separator();
                        }
                    }
                }
                if (separatorAnchor != mSeparatorAnchors.end()) {
                    if (*separatorAnchor == p) {
                        separatorAnchor++;
                    }
                }
                if (groupBeginAnchor != mGroupBeginAnchors.end()) {
                    if (*groupBeginAnchor == p || *groupBeginAnchor == nullptr) {
                        groupsVisibleStack.push_back(ImGui::CollapsingHeader((*groupNamesIt++ + "##" + suffix + "__group_" + p->getName()).c_str() ,
                                    ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen));
                        groupBeginAnchor++;
                    }
                }
                if (groupEndAnchor != mGroupEndAnchors.end()) {
                    if (*groupEndAnchor == p) {
                        if (groupsVisibleStack.back() == true) {
                            // If group is visible add a spacing to mark the end of the group
                            ImGui::Separator();
                        }
                        groupsVisibleStack.pop_back();
                        groupEndAnchor++;
                    }
                }
            }

        }
    }
//    ImGui::End(); // End the window
    if (mManageIMGUI) {
        end();
    }
}

void ControlGUI::init(int x, int y) {
    static int id = 0;
    mId = id++;
    mX = x;
    mY = y;

    mSequencerItems = (char **) malloc(32 * sizeof(char *));
    for (size_t i = 0; i < 32; i++) {
        mSequencerItems[i] = (char *) malloc(32 * sizeof(char));
    }

    if (mManageIMGUI) {
        initIMGUI();
    }
}

void ControlGUI::begin() {
    string name = "__ControlGUI_" + std::to_string(mId);
    beginIMGUI_minimal(true, name.c_str(), mX, mY, mGUIBackgroundAlpha);
}

void ControlGUI::end() {
    endIMGUI_minimal(true);
}

void ControlGUI::cleanup() {
    if (mManageIMGUI) {
        shutdownIMGUI();
    }
    for (size_t i = 0; i < 32; i++) {
        free(mSequencerItems[i]);
    }
    free(mSequencerItems);
}

void ControlGUI::drawPresetHandler()
{
     if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {

        std::map<int, std::string> presets = mPresetHandler->availablePresets();
        static int selection = -1;
        std::string currentPresetName = mPresetHandler->getCurrentPresetName();

        char buf1[64];
        strncpy(buf1, currentPresetName.c_str(), 64);
        if (ImGui::InputText("preset##__Preset", buf1, 64)) {
            currentPresetName = buf1;
        }
        static int presetHandlerBank = 0;
        int numColumns = 12;
        int numRows = 4;
        int counter = presetHandlerBank * (numColumns * numRows) ;
        std::string suffix = "##__Preset"; 
        for (int row = 0; row < numRows; row++) {
            for (int column = 0; column < numColumns; column++) {
                std::string name = std::to_string(counter);
                ImGui::PushID(counter);

                bool is_selected = selection == counter;
                if (is_selected) {
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1, 0.1, 0.1, 1.0));
                }
                if (ImGui::Selectable((name + suffix).c_str(), is_selected, 0, ImVec2(18, 15)))
                {
                    if (mStoreButtonOn) {
                        std::string saveName = currentPresetName;
                        if (saveName.size() == 0) {
                            saveName = name;
                        } 
                        mPresetHandler->storePreset(counter, name.c_str());
                        selection = counter;
                        mStoreButtonOn = false;
                    } else {
                        if (mPresetHandler->recallPreset(counter) != "") { // Preset is available
                            selection = counter;
                        }
                    }
                }
                if (is_selected) {
                    ImGui::PopStyleColor(1);
                }
                if (column < numColumns - 1) ImGui::SameLine();
                counter++;
                ImGui::PopID();
            }
        }
        ImGui::Checkbox("Store##__Preset", &mStoreButtonOn);
        ImGui::SameLine();
        static std::vector<string> seqList {"1", "2", "3", "4"};
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
        ImGui::Combo("Bank##__Preset", &presetHandlerBank, vector_getter, static_cast<void*>(&seqList), seqList.size());
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
        float morphTime = mPresetHandler->getMorphTime();
        if (ImGui::InputFloat("morph time##__Preset", &morphTime, 0.0f, 20.0f)) {
            mPresetHandler->setMorphTime(morphTime);
        }
        ImGui::PopItemWidth();
//            ImGui::Text("%s", currentPresetName.c_str());

    }
}

void ControlGUI::drawSequenceRecorder()
{
    if (ImGui::CollapsingHeader("Sequence Recorder##__SequenceRecorder", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
        static char buf_seq_recorder[64] = "test"; ImGui::InputText("Record Name##__SequenceRecorder", buf_seq_recorder, 64);
        static bool button_seq_recorder_value = false;
        if (ImGui::Checkbox("Record##__SequenceRecorder", &button_seq_recorder_value)) {
            if (button_seq_recorder_value) {
                mSequenceRecorder->startRecord(buf_seq_recorder, mOverwriteButtonValue);
            } else {
                mSequenceRecorder->stopRecord();
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("Overwrite##__SequenceRecorder", &mOverwriteButtonValue);
    }
}

void ControlGUI::drawParameterMeta(ParameterMeta *param, string suffix)
{
    if (strcmp(typeid(*param).name(), typeid(ParameterBool).name() ) == 0) { // ParameterBool
        ParameterBool *p = dynamic_cast<ParameterBool *>(param);

        drawParameterBool(p, suffix);
    } else if (strcmp(typeid(*param).name(), typeid(Parameter).name()) == 0) {// Parameter
        Parameter *p = dynamic_cast<Parameter *>(param);
        drawParameter(p, suffix);
    } else if (strcmp(typeid(*param).name(), typeid(ParameterPose).name()) == 0) {// ParameterPose
        ParameterPose *p = dynamic_cast<ParameterPose *>(param);
        drawParameterPose(p);
    } else if (strcmp(typeid(*param).name(), typeid(ParameterMenu).name()) == 0) {// ParameterMenu
        ParameterMenu *p = dynamic_cast<ParameterMenu *>(param);
        drawMenu(p, suffix);
    }
    else if (strcmp(typeid(*param).name(), typeid(ParameterChoice).name()) == 0) {// ParameterChoice
        ParameterChoice *p = dynamic_cast<ParameterChoice *>(param);
        drawChoice(p, suffix);
    }
    else if (strcmp(typeid(*param).name(), typeid(ParameterVec3).name()) == 0) {// ParameterVec3
        ParameterVec3 *p = dynamic_cast<ParameterVec3 *>(param);
        drawVec3(p, suffix);
    }
    else {
        // TODO this check should be performed on registration
        std::cout << "Unsupported Parameter type for display" << std::endl;
    }
}

void ControlGUI::drawParameter(Parameter * param, string suffix)
{
	if (param->getHint("intcombo") == 1.0) {
		int value = (int)param->get();
		vector<string> values;
		for (float i = param->min(); i <= param->max(); i++) {
			// There's got to be a better way...
			values.push_back(to_string((int)i));
		}
		auto vector_getter = [](void* vec, int idx, const char** out_text)
		{
			auto& vector = *static_cast<std::vector<std::string>*>(vec);
			if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
			*out_text = vector.at(idx).c_str();
			return true;
		};
		if (!values.empty()) {
			bool changed = ImGui::Combo((param->getName() + suffix).c_str(), &value, vector_getter,
				static_cast<void*>(&values), values.size());
			if (changed) {
				param->set(value);
			}
		}
	}
	else {
		float value = param->get();
		bool changed = ImGui::SliderFloat((param->getName() + suffix).c_str(), &value, param->min(), param->max());
		if (changed) {
			param->set(value);
		}
	}
}

void ControlGUI::drawParameterBool(ParameterBool * param, string suffix)
{
	bool changed;
	if (param->getHint("latch") == 1.0) {
		bool value = param->get() == 1.0;
		changed = ImGui::Checkbox((param->getName() + suffix).c_str(), &value);
		if (changed) {
			param->set(value ? 1.0 : 0.0);
		}
	}
	else {
		changed = ImGui::Button((param->getName() + suffix).c_str());
		if (changed) {
			param->set(1.0);
		}
		else {
			if (param->get() == 1.0) {
				param->set(0.0);
			}
		}
	}
}

void ControlGUI::drawParameterPose(ParameterPose *pose)
{
	if (ImGui::CollapsingHeader(("Pose:" + pose->getName()).c_str(), ImGuiTreeNodeFlags_CollapsingHeader)) {
		Vec3d &currentPos = pose->get().pos();
		float x = currentPos.elems()[0];
		if (ImGui::SliderFloat(("X##" + pose->getName()).c_str(), &x, -5, 5)) {
			currentPos.elems()[0] = x;
			pose->set(Pose(currentPos, pose->get().quat()));
		}
		float y = currentPos.elems()[1];
		if (ImGui::SliderFloat(("Y##" + pose->getName()).c_str(), &y, -5, 5)) {
			currentPos.elems()[1] = y;
			pose->set(Pose(currentPos, pose->get().quat()));
		}
		float z = currentPos.elems()[2];
		if (ImGui::SliderFloat(("Z##" + pose->getName()).c_str(), &z, -10, 0)) {
			currentPos.elems()[2] = z;
			pose->set(Pose(currentPos, pose->get().quat()));
		}
		ImGui::Spacing();
	}
}

void ControlGUI::drawNav()
{
	if (ImGui::CollapsingHeader("Navigation##nav", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
		Vec3d &currentPos = mNav->pos();
		float x = currentPos.elems()[0];

        bool changed = ImGui::SliderFloat("X##__nav_", &x, -10, 10);
		if (changed) {
			currentPos.elems()[0] = x;
		}
		float y = currentPos.elems()[1];
        changed = ImGui::SliderFloat("Y##__nav_", &y, -10, 10);
		if (changed) {
			currentPos.elems()[1] = y;
		}
		float z = currentPos.elems()[2];
        changed = ImGui::SliderFloat("Z##__nav_", &z, -10, 10);
		if (changed) {
			currentPos.elems()[2] = z;
		}
		ImGui::Spacing();
	}
}

void ControlGUI::drawMenu(ParameterMenu * param, std::string suffix)
{
	int value = param->get();
	auto values = param->getElements();
	bool changed = ImGui::Combo((param->getName() + suffix).c_str(), &value, vector_getter,
				static_cast<void*>(&values), values.size());
	if (changed) {
		param->set(value);
	}
}

void ControlGUI::drawChoice(ParameterChoice * param, std::string suffix)
{
    uint16_t value = param->get();
    auto elements = param->getElements();
    if (ImGui::CollapsingHeader((param->getName() + suffix).c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
        for (unsigned int i = 0; i < elements.size(); i++) {
            bool state = value & (1 << i);
            if (ImGui::Checkbox((elements[i] + "##" + suffix + param->getName()).c_str(), &state)) {
                value ^= (-(state) ^ value) & (1UL << i); // Set an individual bit
                param->set(value);
            }

        }
    }

}

void ControlGUI::drawVec3(ParameterVec3 *param, string suffix)
{
    if (ImGui::CollapsingHeader((param->getName() + suffix).c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
        Vec3f currentValue = param->get();
        float x = currentValue.elems()[0];
        bool updated = false;
        bool changed = ImGui::SliderFloat(("X##" + suffix + param->getName()).c_str(), &x, -10, 10);
        if (changed) {
            currentValue.x = x;
            updated = true;
        }
        float y = currentValue.elems()[1];
        changed = ImGui::SliderFloat(("Y##" + suffix + param->getName()).c_str(), &y, -10, 10);
        if (changed) {
            currentValue.y = y;
            updated = true;
        }
        float z = currentValue.elems()[2];
        changed = ImGui::SliderFloat(("Z##" + suffix + param->getName()).c_str(), &z, -10, 10);
        if (changed) {
            currentValue.z = z;
            updated = true;
        }
        if (updated) {
            param->set(currentValue);
        }
    }
}

void ControlGUI::drawDynamicScene(DynamicScene *scene, std::string suffix)
{
    
}

void ControlGUI::drawBundleGroup(std::vector<ParameterBundle *> bundleGroup, string suffix)
{
    std::string name = bundleGroup[0]->name();
    ImGui::Separator();
    ImGui::Text(name.c_str());
    ImGui::SameLine();
    int index = mCurrentBundle[name];
    if (ImGui::InputInt(suffix.c_str(), &index)) {
        if (index >= 0 && index < (int) bundleGroup.size()) {
            mCurrentBundle[name] = index;
        }
    }
    suffix += "__index_" + std::to_string(index);
    for (ParameterMeta *param: bundleGroup[mCurrentBundle[name]]->parameters()) {
        drawParameterMeta(param, suffix);
    }
    ImGui::Separator();

}

ControlGUI &ControlGUI::registerParameterMeta(ParameterMeta &param) {
    std::string group = param.getGroup();
    if (mElements.find(group) == mElements.end()) {
		mElements[group] = std::vector<ParameterMeta *>();
    }
	mElements[group].push_back(&param);
    mLatestElement = &param;
    return *this;
}

ControlGUI &ControlGUI::registerParameterBundle(ParameterBundle &bundle)
{
    std::string bundleName = bundle.name();
    if (mBundles.find(bundleName) == mBundles.end()) {
        mBundles[bundleName] = std::vector<ParameterBundle *>();
        mCurrentBundle[bundleName] = 0;
    }
    mBundles[bundleName].push_back(&bundle);
    return *this;
}

ControlGUI &ControlGUI::registerNav(Nav &nav)
{
    mNav = &nav;
    return *this;
}

ControlGUI &ControlGUI::registerPresetHandler(PresetHandler &presetHandler) {
    mPresetHandler = &presetHandler;
    return *this;
}

ControlGUI &ControlGUI::registerPresetSequencer(PresetSequencer &presetSequencer) {
    mPresetSequencer = &presetSequencer;
    return *this;
}

ControlGUI &ControlGUI::registerSequenceRecorder(SequenceRecorder &recorder)
{
    mSequenceRecorder = &recorder;
    return *this;
}

ControlGUI &ControlGUI::registerSynthRecorder(SynthRecorder &recorder) {
    mSynthRecorder = &recorder;
    return *this;
}

ControlGUI &ControlGUI::registerSynthSequencer(SynthSequencer &seq) {
    mSynthSequencer = &seq;
    mPolySynth = &seq.synth();
    return *this;
}

ControlGUI &ControlGUI::registerDynamicScene(DynamicScene &scene) {
    mScene = &scene;
    return *this;
}

ControlGUI &ControlGUI::registerMarker(GUIMarker &marker) {
    switch(marker.getType()) {
        case GUIMarker::MarkerType::GROUP_BEGIN:
        mGroupBeginAnchors.push_back(mLatestElement);
        mGroupNames.push_back(marker.getName());
        break;
        case GUIMarker::MarkerType::GROUP_END:
        mGroupEndAnchors.push_back(mLatestElement);
        break;
        case GUIMarker::MarkerType::SEPARATOR:
        mSeparatorAnchors.push_back(mLatestElement);
        break;
    }
    return *this;
}

