#ifndef TERROR_INSANITY_EFFECT_H
#define TERROR_INSANITY_EFFECT_H

#include "Config.h"

namespace terror {
	class InsanityEffect {
	public:
		InsanityEffect(float minTimeToStart, float maxTimeToStart, float executionTime, float minInsanityPoints)
			: effectExecuting(false), timeToFinish(-1.0f), minTimeToStart(minTimeToStart), maxTimeToStart(maxTimeToStart), executionTime(executionTime), minInsanityPoints(minInsanityPoints)
		{
			this->setTimeToStart();
		}

		void update(float diffTime) {
			if (this->effectExecuting) {
				this->timeToFinish -= diffTime;
				this->execute(diffTime);
				if (this->timeToFinish < 0.0f) {
					this->endExecution();
					this->setTimeToStart();
				}
			}else {
				this->timeToStart -= diffTime;
				if (this->timeToStart < 0.0f) {
					this->timeToFinish = this->executionTime;
					this->startExecution();
				}
			}
		}

		void setMaximumTimeToStart(float maxTimeToStart) {
			this->maxTimeToStart = maxTimeToStart;
		}

		void setMinimumTimeToStart(float minTimeToStart) {
			this->minTimeToStart = minTimeToStart;
		}

		float getMaximumTimeToStart() {
			return this->maxTimeToStart;
		}

		float getMinimumTimeToStart() {
			return this->minTimeToStart;
		}

		float getMinimumInsanityToActivate() {
			return this->minInsanityPoints;
		}

		bool isEffectExecuting() {
			return this->effectExecuting;
		}

		void reset() {
			this->setTimeToStart();
			if (this->effectExecuting) {
				this->endExecution();
			}
			this->doAdditionalReset();
		}

		virtual ~InsanityEffect(){}

	protected:
		float timeToStart;
		float timeToFinish;
		float executionTime;
		float minTimeToStart;
		float maxTimeToStart;
		float minInsanityPoints;
		bool effectExecuting;

	private:
		void startExecution() {
			this->effectExecuting = true;
			this->doStartExecution();
		}

		void execute(float diffTime) {
			this->doExecute(diffTime);
		}

		void endExecution() {
			this->effectExecuting = false;
			this->doEndExecution();
		}

		virtual void doStartExecution() = 0;
		virtual void doExecute(float diffTime) = 0;
		virtual void doEndExecution() = 0;

		void setTimeToStart() {
			this->timeToStart = randomNumber(this->minTimeToStart, this->maxTimeToStart);
		}

		virtual void doAdditionalReset() {}
	};

	typedef SharedPointer< InsanityEffect > InsanityEffectPtr;
}

#endif