#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/FLAlertLayer.hpp>

class $modify(FLAlertLayer) {
	struct Fields {
		GameManager* gm = GameManager::get();

		std::unordered_map<std::string, cocos2d::ccColor3B> colorMap = {
			{"xp1", gm->colorForIdx(gm->getPlayerColor())},
			{"xp2", gm->colorForIdx(gm->getPlayerColor2())},
			{"xpg", gm->colorForIdx(gm->getPlayerGlowColor())},						

			{"xa",  ccc3(150, 50, 255)},
			{"xav", ccc3(127, 255, 0)}, 
			{"xb",  ccc3(74, 82, 225)},
			{"xbg",  ccc3(245, 245, 220)},
			{"xc",  ccc3(255, 255, 150)},
			{"xd",  ccc3(255, 150, 255)},
			{"xdr", ccc3(139, 0, 0)}, 
			{"xf",  ccc3(150, 255, 255)},
			{"xg",  ccc3(64, 227, 72)},
			{"xgr", ccc3(128, 128, 128)},
			{"xh", ccc3(34, 139, 34)},
			{"xj",  ccc3(50, 200, 255)},
			{"xk",  ccc3(0, 0, 0)},
			{"xl",  ccc3(96, 171, 239)},
			{"xnv", ccc3(25, 25, 112)},
			{"xo",  ccc3(255, 165, 75)},
			{"xob", ccc3(255, 100, 0)}, 
			{"xp",  ccc3(255, 0, 255)},
			{"xr",  ccc3(255, 90, 90)},
			{"xrb",  ccc3(255, 0, 0)},
			{"xro", ccc3(220, 20, 60)},
			{"xs",  ccc3(255, 220, 65)},
			{"xsl", ccc3(192, 192, 192)},
			{"xv", ccc3(186, 85, 211)},
			{"xy",  ccc3(255, 255, 0)},

			{"xrnbw", ccc3(255, 255, 255)}
		};

		std::vector<cocos2d::ccColor3B> rainbowColors = {
			ccc3(255, 90, 90),
			ccc3(255, 165, 75),
			ccc3(255, 220, 65),
			ccc3(64, 227, 72),
			ccc3(96, 171, 239),
			ccc3(150, 50, 255)
		};

	};

	struct ColorTag {
		std::string name;
		size_t start;
		size_t end;
	};

	bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
		std::string newDesc;
		std::vector<ColorTag> tags;

		size_t pos = 0;
		cocos2d::ccColor3B currentColor = cocos2d::ccc3(255, 255, 255);

		while (pos < p2.length()) {
			auto tagStart = p2.find("<", pos);
			if (tagStart == std::string::npos) {
				newDesc += p2.substr(pos);
				break;
			}

			newDesc += p2.substr(pos, tagStart - pos);

			auto tagEnd = p2.find(">", tagStart);
			if (tagEnd == std::string::npos) break;

			std::string tagContent = p2.substr(tagStart + 1, tagEnd - tagStart - 1);

			if (tagContent.starts_with("x")) {
				if (!tagContent.empty() && m_fields->colorMap.contains(tagContent)) {
					size_t colorStart = newDesc.length();
					pos = tagEnd + 1;

					auto closeTag = p2.find("</x>", pos);
					std::string inner = p2.substr(pos, closeTag - pos);
					newDesc += inner;

					tags.push_back(ColorTag{
						.name = tagContent,
						.start = colorStart,
						.end = colorStart + inner.length()
						});

					pos = closeTag != std::string::npos ? closeTag + 4 : p2.length();
					continue;
				}
			}

			newDesc += p2.substr(tagStart, tagEnd - tagStart + 1);
			pos = tagEnd + 1;
		}

		if (!FLAlertLayer::init(p0, p1, newDesc, p3, p4, p5, p6, p7, p8)) return false;

		auto textArea = m_mainLayer->getChildByID("content-text-area");
		if (!textArea) return true;

		auto mbf = textArea->getChildByType<MultilineBitmapFont>(0);
		if (!mbf) return true;

		for (const auto& tag : tags) {
			size_t globalIndex = 0;

			if (tag.name == "xrnbw") {
				for (auto label : CCArrayExt<CCLabelBMFont*>(mbf->getChildren())) {
					if (!label) continue;

					auto glyphs = CCArrayExt<CCFontSprite*>(label->getChildren());
					for (auto glyph : glyphs) {
						if (globalIndex >= tag.start && globalIndex < tag.end) {
							if (glyph) {
								size_t colorIdx = (globalIndex - tag.start) % m_fields->rainbowColors.size();
								glyph->setColor(m_fields->rainbowColors[colorIdx]);
							}
						}
						globalIndex++;
						if (globalIndex >= tag.end) break;
					}
					if (globalIndex >= tag.end) break;
				}
			}
			else {
				auto it = m_fields->colorMap.find(tag.name);
				cocos2d::ccColor3B tagColor = (it != m_fields->colorMap.end()) ? it->second : cocos2d::ccc3(255, 0, 0);

				for (auto label : CCArrayExt<CCLabelBMFont*>(mbf->getChildren())) {
					if (!label) continue;

					auto glyphs = CCArrayExt<CCFontSprite*>(label->getChildren());
					for (auto glyph : glyphs) {
						if (globalIndex >= tag.start && globalIndex < tag.end) {
							if (glyph) glyph->setColor(tagColor);
						}
						globalIndex++;
						if (globalIndex >= tag.end) break;
					}
					if (globalIndex >= tag.end) break;
				}
			}
		}

		return true;
	}
};
