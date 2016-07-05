/*
* =====================================================================================
*
*       Filename:  Scheduling.cpp
*
*    Description:  调度模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<vector>
#include"Schedule.h"
#include"System.h"
#include"Exception.h"
#include"Definition.h"
#include"Global.h"

using namespace std;


void cSystem::scheduling() {
	//清除上一次调度信息
	scheduleInfoClean();

	schedulePF_RP_CSI_UL();
	
}



void cSystem::scheduleInfoClean() {
	for (ceNB _eNB : m_VeceNB) {
		if (m_Config.linkType == UPLINK)
			_eNB.m_vecScheduleInfo.clear();
	}
}


//void cSystem::feedbackInfoReceived() {
//	for (ceNB &_eNB : m_VeceNB) {//遍历所有基站
//		if (m_Config.linkType == DOWNLINK) {
//			_eNB.vecToBeScheduleVUEDL.clear();
//			for (int sector_ID : _eNB.m_VecSector) {//遍历该基站中的所有扇区
//				for (int _VUE_ID : m_VecSector[sector_ID].admitUserIdListDL)//遍历该扇区的接纳链表中的所有用户
//					_eNB.vecToBeScheduleVUEDL.push_back(_VUE_ID);
//			}
//		}
//		else if (m_Config.linkType == UPLINK) {
//			_eNB.vecToBeScheduleVUEUL.clear();
//			for (int sector_ID : _eNB.m_VecSector) {//遍历该基站中的所有扇区
//				for (int _VUE_ID : m_VecSector[sector_ID].admitUserIdListUL)//遍历该扇区的接纳链表中的所有用户
//					_eNB.vecToBeScheduleVUEUL.push_back(_VUE_ID);
//			}
//		}
//	}
//}


void cSystem::schedulePF_RP_CSI_UL() {
	for (ceNB& _eNB : m_VeceNB) {//对每一个基站进行一次调度
		int k = _eNB.m_VecRSU.size();
		vector<vector<bool>> SPU(k, vector<bool>(gc_SubbandNum));//每个RSU用一个vector<bool>来管理其SPU，true代表已选如该子带
		vector<int> S;//未分配的子带集合(存储子带的ID）
		
		//初始化子带集合S（这里需要不需要？每次调度前应该没有子带被占用吧）
		for (int subbandId = 0; subbandId < gc_SubbandNum; subbandId++) 
			if (_eNB.m_UnassignedSubband)S.push_back(subbandId);
		

		//计算每个RSU对应不同子带的PF因子
		vector<PFInfo> F;//存储PF因子的容器
		for (int RSUId : _eNB.m_VecRSU) {
			for (int subbandId = 0; subbandId < gc_SubbandNum; subbandId++) {
				if (_eNB.m_UnassignedSubband[subbandId] == false) continue;//该子带已被分配
				double t_FactorPF= log10(1 + m_VecRSU[RSUId].m_SINR[subbandId]) / m_VecRSU[RSUId].m_AccumulateThroughput;
				F.push_back(PFInfo(RSUId, subbandId, t_FactorPF));
			}
		}

		//开始排序算法
		int p = 1;
		while (S.size() != 0) {
			PFInfo pPFInfo = selectKthPF(F, p, 0, F.size() - 1);
			int ue = pPFInfo.RSUId;
			int sub = pPFInfo.SubbandId;

			if (SPU[ue].size() == 0 || (sub>0 && SPU[ue][sub - 1] || sub < gc_SubbandNum - 1 && SPU[ue][sub + 1])) {
				S.erase(S.begin() + sub);
				SPU[ue][sub] = true;
				F.erase(F.begin() + p - 1);//这里注意，p代表的是顺序数，从1开始计
			}
			else {
				p++;
			}
		}

		//写入调度结果：
		for (int RSUId = 0; RSUId < k; RSUId++) {
			sScheduleInfo curRSUScheduleInfo;
			curRSUScheduleInfo.userId = RSUId;
			for (int subbandId = 0; subbandId < gc_SubbandNum; subbandId++) {
				if (SPU[RSUId][subbandId]) curRSUScheduleInfo.assignedSubbandId.push_back(subbandId);
			}
			if (curRSUScheduleInfo.assignedSubbandId.size() != 0) _eNB.m_vecScheduleInfo.push_back(curRSUScheduleInfo);
		}
	}
}


PFInfo cSystem::selectKthPF(std::vector<PFInfo>& v, int k,int p,int r) {
	if (p == r) return v[p];
	int q = partition(v, p, r);
	int n = q - p + 1;
	if (n == k) return v[q];
	else if (n > k) return  selectKthPF(v, k, p, q - 1);
	else return selectKthPF(v, k - n, q + 1, r);		
}

int cSystem::partition(std::vector<PFInfo>& v, int p, int r) {
	int k = p - 1;
	double x = v[r].FactorPF;
	for (int j = p; j <= r - 1; j++) {
		if (v[j].FactorPF < x) {
			k++;
			exchange(v, k, j);
		}
	}
	exchange(v, k + 1, r);
	return k + 1;
}

void cSystem::exchange(std::vector<PFInfo>& v, int i, int j) {
	PFInfo tem = v[i];
	v[i] = v[j];
	v[j] = tem;
}



//void System::feedbackInfoReceived() {
//	for (int sectorIdx = 0; sectorIdx < config.sectorNum; sectorIdx++) {
//		for (list<int>::const_iterator it = vecSector[sectorIdx].admitUserIdList_DL.begin(); it != vecSector[sectorIdx].admitUserIdList_DL.end(); it++) {
//			int usrId = *it;//遍历该扇区中被接纳用户链表
//			vecVUE[usrId].feedback_eNB = vecVUE[usrId].feedbackRead_DL;
//		}
//	}
//}

///*
//问题，谁的延时，
//*/
//void System::feedbackInfoDelay() {
//	for (int sectorIdx = 0; sectorIdx < config.sectorNum; sectorIdx++) {
//		Sector curSector = vecSector[sectorIdx];
//		if (config.linkType == DOWNLINK) 		
//			for (list<int>::const_iterator it = curSector.admitUserIdList_DL.begin(); it != curSector.admitUserIdList_DL.end(); ++it)
//				vecVUE[*it].feedbackInfoDelay(DOWNLINK);
//		else if (config.linkType == UPLINK) 		
//			for (list<int>::const_iterator it = curSector.admitUserIdList_UL.begin(); it != curSector.admitUserIdList_UL.end(); ++it)
//				vecVUE[*it].feedbackInfoDelay(UPLINK);
//	}
//}

/*--------------------------------
将feedback的信息拷贝到feedbackWrite_DL中
---------------------------------*/
//void System::feedbackInfoSent() {
//	for (int sectorIdx = 0; sectorIdx < config.sectorNum; sectorIdx++) {
//		for (list<int>::const_iterator it = vecSector[sectorIdx].admitUserIdList_DL.begin(); it != vecSector[sectorIdx].admitUserIdList_DL.end(); it++) {
//			int usr = *it;
//			vecVUE[usr].feedbackWrite_DL = vecVUE[usr].feedback;
//		}
//	}
//}
//
//


//void System::scheduleDistribute() {
//	for (int sectorIdx = 0; sectorIdx != config.sectorNum; ++sectorIdx) {
//		if (config.linkType == UPLINK) {	
//			for (int& userId : vecSector[sectorIdx].admitUserIdList_UL) {
//				vecVUE[userId].isScheduled_UL = false;
//				vecVUE[userId]._H_Indicator_UL.clear();
//				memset(vecVUE[userId]._RBs, 0, sizeof(vecVUE[userId]._RBs));
//				vecVUE[userId].servingSet.clear();
//			}
//			for (int _H_Idx = 0; _H_Idx != config._H_Num; ++_H_Idx) {
//				for (int userIdx = 0; userIdx < g_c_Max_MU_MIMO_UserNum; ++userIdx) {
//					int userId = vecSector[sectorIdx].scheduleInfoRead_UL[_H_Idx].userId[userIdx];
//
//					if (userId != INVALID_USER) {
//						//更新用户mcs和harq进程
//						if (!vecVUE[userId].isScheduled(UPLINK))
//							vecVUE[userId].isScheduled_UL = true;
//
//						vecVUE[userId]._H_Indicator_UL.push_back(_H_Idx);
//						vecVUE[userId]._RBs[0]++;
//					}
//				}
//			}
//			for (int& userIdlist: vecSector[sectorIdx].admitUserIdList_UL)
//				vecVUE[userIdlist]._RBs[0] = vecVUE[userIdlist]._RBs[0] / config._H_NumPer_RB;
//		}
//
//		if (config.linkType == DOWNLINK) {
//			for (int &userId : vecSector[sectorIdx].admitUserIdList_DL) {
//				vecVUE[userId].isScheduled_DL = false;
//				vecVUE[userId]._H_Indicator_DL.clear();
//				vecVUE[userId].codewords = vecVUE[userId].feedback_eNB.codewords;
//				vecVUE[userId]._RI_Current = vecVUE[userId].feedback_eNB._RI;
//				memset(vecVUE[userId]._RBs, 0, sizeof(vecVUE[userId]._RBs));
//				vecVUE[userId].servingSet.clear();
//				vecVUE[userId].servingSet.AppendSector(sectorIdx);
//			}
//			for (int H_idx = 0; H_idx != config._H_Num; ++H_idx) {
//				for (int userIdx = 0; userIdx != g_c_Max_MU_MIMO_UserNum; ++userIdx) {
//					int userId = vecSector[sectorIdx].scheduleInfoRead_DL[H_idx].userId[userIdx];
//
//					if (userId != INVALID_USER) {
//						if (vecVUE[userId].servingSet.IsServing(sectorIdx) == false)
//							vecVUE[userId].servingSet.AppendSector(sectorIdx);
//
//						if (!vecVUE[userId].isScheduled(DOWNLINK))
//							vecVUE[userId].isScheduled_DL = true;
//						vecVUE[userId]._H_Indicator_DL.push_back(H_idx);
//					}
//				}
//			}
//			for (int& userId : vecSector[sectorIdx].admitUserIdList_DL) {
//				for (int codeword_idx = 0; codeword_idx != vecVUE[userId].codewords; codeword_idx++) {
//					int layers = g_LUT_Codewords_2_LayersNum(config.transType, vecVUE[userId].feedback_eNB._RI, vecVUE[userId].feedback_eNB.codewords, codeword_idx);
//					vecVUE[userId]._RBs[codeword_idx] = vecVUE[userId]._H_Indicator_DL.size()*layers / config._H_NumPer_RB;
//				}
//			}
//		}
//	}
//}
//
//
//void System::schedule_MCS() {
//	int subframe_cata;
//	int _MCS_Restriction;
//	double lp_abs_bias;
//
//	for (int sectorIdx = 0; sectorIdx != config.sectorNum; sectorIdx++) {
//		_MCS_Restriction = 30;
//		lp_abs_bias = 0;
//		if (config.linkType == DOWNLINK) {
//				list<int>::const_iterator it;
//				for (it = vecSector[sectorIdx].admitUserIdList_DL.begin(); it != vecSector[sectorIdx].admitUserIdList_DL.end(); ++it) {
//					int usr = *it;
//					int sched_wide = 0;
//					vector<double> sinr_codeword(g_c_MaxCodewords, 0.0);
//					vector<double> sinr_unq_codeword(g_c_MaxCodewords, 0.0);
//					for (int hIdx = 0; hIdx < config._H_Num; hIdx++) {
//
//						for (int i = 0; i < vecSector[sectorIdx].scheduleInfoWrite_DL[hIdx].userNum; i++) {
//							if (vecSector[sectorIdx].scheduleInfoWrite_DL[hIdx].userId[i] == usr) {
//								for (int codewords = 0; codewords != vecVUE[usr].feedback_eNB.codewords; codewords++) {
//									int subband_idx = _H_2_Subband(hIdx);
//									sinr_codeword[codewords] += positiveNumber_2_dB(vecVUE[usr]._CQI_PredictRealistic[subband_idx + codewords*config.subbandNum]);
//								}
//								sched_wide++;
//							}
//						}
//					}
//					if (sched_wide != 0) {
//						for (int codewords = 0; codewords != vecVUE[usr].feedback_eNB.codewords; codewords++) {
//							int mcs_idx;
//							int mcs;
//							vecVUE[usr].codesword_sinr_mcs[codewords] = _dB_PositiveNumber(sinr_codeword[codewords] / sched_wide + lp_abs_bias);
//							vecVUE[usr].codesword_unq_sinr_mcs[codewords] = _dB_PositiveNumber(sinr_unq_codeword[codewords] / sched_wide + lp_abs_bias);
//							mcs_idx = g_LUT_Search_MCS_Index(DOWNLINK, vecVUE[usr].codesword_sinr_mcs[codewords], BLER_TARGET);
//
//
//							mcs_idx = CHOOSE_MCS(mcs_idx);
//							mcs = MIN(mcs_idx, _MCS_Restriction);
//							if (mcs <0)
//								mcs = 0;
//							else if (mcs>DOWNLINK_MCS_LEVELS - 1)
//								mcs = DOWNLINK_MCS_LEVELS - 1;
//							vecVUE[usr].MCS[codewords] = mcs;
//						}
//					}
//				}
//			}
//	}
//}
//
//
//void System::scheduleInfoDelay() {
//	for (int sectorIdx = 0; sectorIdx != config.sectorNum; sectorIdx++) {
//		Sector curSector = vecSector[sectorIdx];
//		if (config.linkType == DOWNLINK) {
//			rotate(curSector.schedule_DL.begin(), curSector.schedule_DL.begin() + 1, curSector.schedule_DL.end());
//			curSector.scheduleInfoRead_DL = curSector.schedule_DL.front();
//			curSector.scheduleInfoWrite_DL = curSector.schedule_DL.back();
//		}
//		if (config.linkType == UPLINK) {
//			rotate(curSector.schedule_UL.begin(), curSector.schedule_UL.begin() + 1, curSector.schedule_UL.end());
//			curSector.scheduleInfoRead_UL = curSector.schedule_UL.front();
//			curSector.scheduleInfoWrite_UL = curSector.schedule_UL.back();
//		}
//	}
//}


//void Schedule::schedule_PF_DL_CSI() {
//	for (int sectorIdx = 0; sectorIdx != config.sectorNum; sectorIdx++) {
//		list<int> scheduleList;//每个扇区的调度链表
//
//		scheduleList = vecSector[sectorIdx].admitUserIdList_DL;
//
//		for (int subs = 0; subs != config.subbandNum; subs++) {
//			list<int> select;
//			list<int> unselect = scheduleList;
//			int maxCapUser = INVALID_USER;
//			double maxCap = 0;
//			double maxFactor = EPS;
//			for (list<int>::const_iterator it = unselect.begin(); it != unselect.end(); it++) {
//				//PF
//				int user = *it;
//
//				double tmpSum = 0;
//				//平均码字载干比
//				for (int codewords = 0; codewords < vecVeUE[user].feedback_BS.codewords; codewords++) {
//					int _MCS_Idx;
//					double ave_sinr = vecVeUE[user].feedback_BS._CQI_Subband[subs + codewords*config.subbandNum];
//					_MCS_Idx = g_LUT_Search_MCS_Index(DOWNLINK, ave_sinr, BLER_TARGET);
//					tmpSum += g_LUT_MCS_Index_2_Effective(_MCS_Idx, DOWNLINK);
//				}
//				double factor = tmpSum / (vecVeUE[*it].avgThroughput + EPS);
//				//double factor = tmp_cap;
//				if (factor>maxFactor) {
//					maxCap = tmpSum;
//					maxCapUser = *it;
//					maxFactor = factor;
//				}
//			}
//			if (maxCapUser != INVALID_USER) {
//
//				//比较每个流上的载干比
//
//				vector<double> cqi_ideal_on_codeword(g_c_MaxCodewords, 0.0);
//				Matrix w = codebook.GetPrecodingMatrix(config.anBS, vecVeUE[maxCapUser].feedback_BS._PMI, vecVeUE[maxCapUser].feedback_BS._RI);
//				for (int h_idx = subbandInfo[subs].begin; h_idx != subbandInfo[subs].end; h_idx++) {
//					Matrix H = GetChannel(DOWNLINK, maxCapUser, sectorIdx, h_idx);
//					Matrix Rii = vecVeUE[maxCapUser].interference[h_idx];
//					vector<double> SINR = SINR_SM_MMSE(H, w, Rii, Rii);
//					for (int layer = 0; layer != vecVeUE[maxCapUser].feedback_BS._RI; layer++) {
//						int codeword_idx = g_LUT_Layer_2_Codeword(config.trans_mode, vecVeUE[maxCapUser].feedback_BS._RI, vecVeUE[maxCapUser].feedback_BS.codewords, layer);
//						cqi_ideal_on_codeword[codeword_idx] += ABS_2_DB(SINR[layer]);
//					}
//				}
//
//
//				for (int codeword_idx = 0; codeword_idx != vecVeUE[maxCapUser].feedback_BS.codewords; codeword_idx++) {
//					int layers = g_LUT_Codewords_2_LayersNum(config.trans_mode, vecVeUE[maxCapUser].feedback_BS[subframeCata].RI, vecVeUE[maxCapUser].feedback_BS[subframeCata].codewords, codeword_idx);
//					double sinr = DB_2_ABS(cqi_ideal_on_codeword[codeword_idx] / layers / subbandInfo[subs].size);
//					vecVeUE[maxCapUser].CQI_predict_ideal[subs + codeword_idx*config.subbandNum] = sinr;
//					vecVeUE[maxCapUser].CQI_predict_realistic[subs + codeword_idx*config.subbandNum] = vecVeUE[maxCapUser].feedback_BS[subframeCata].CQI_subband[subs + codeword_idx*config.subband_num];
//				}
//
//
//
//				for (int h_idx = subbandInfo[subs].begin; h_idx != subbandInfo[subs].end; h_idx++) {
//					vecSector[sectorIdx].scheduleInfoWrite_DL[h_idx].userId[0] = maxCapUser;
//					vecSector[sectorIdx].scheduleInfoWrite_DL[h_idx].userNum = 1;
//					vecSector[sectorIdx].scheduleInfoWrite_DL[h_idx].eType = CODEBOOK;
//					vecSector[sectorIdx].abs_codebook_idx[h_idx] = COMPACT_CSI(vecVeUE[maxCapUser].feedback_BS[subframeCata].PMI, vecVeUE[maxCapUser].feedback_BS[subframeCata].RI);
//				}
//			}
//		}
//	}
//}


